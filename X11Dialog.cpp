// X11Dialog.cpp
// Диалоговое окно на чистом XCB: заголовок "Ошибка", текст "Успех", кнопка "OK".
// Отрисовка текста/кнопки — Cairo поверх XCB-поверхности (без GTK, без Xlib).
//
// Сборка:
//   g++ X11Dialog.cpp -o X11Dialog -lxcb -lcairo -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0
//
// Запуск:
//   ./X11Dialog

#include <xcb/xcb.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <pango/pangocairo.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <unistd.h>

// ============================================================
// Встроенная PNG-иконка (создаётся на этапе линковки через objcopy)
// Сборка: objcopy -I binary -O elf64-x86-64 -B i386:x86-64 \
//         icons8-error-64.png icons8-error-64.o
// ============================================================
extern "C" {
extern const uint8_t _binary_icons8_error_64_png_start[];
extern const uint8_t _binary_icons8_error_64_png_end[];
}

// ============================================================
// Глобальное состояние окна
// ============================================================

static xcb_connection_t* g_conn  = nullptr;
static xcb_screen_t*     g_screen = nullptr;
static xcb_window_t      g_window = 0;
static bool              g_running = false;

static const int kWindowWidth  = 340;
static const int kWindowHeight = 180;

// Атом для обработки крестика (WM_DELETE_WINDOW)
static xcb_atom_t g_wm_delete = 0;
static xcb_atom_t g_net_wm_name = 0;
static xcb_atom_t g_utf8_string = 0;

// Найти visualtype по ID (root_visual) среди глубин экрана
static xcb_visualtype_t* FindVisual(xcb_screen_t* screen) {
    xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t vis_iter = xcb_depth_visuals_iterator(depth_iter.data);
        for (; vis_iter.rem; xcb_visualtype_next(&vis_iter)) {
            if (vis_iter.data->visual_id == screen->root_visual) {
                return vis_iter.data;
            }
        }
    }
    return nullptr;
}

// Отладка: принудительно проталкивает отрисовку на сервер
static void FlushDraw(cairo_surface_t* surface) {
    cairo_surface_flush(surface);
    xcb_flush(g_conn);
}

// Геометрия кнопки OK (для проверки клика)
static const int kOkX      = (340 - 90) / 2;   // по центру
static const int kOkY      = 110;
static const int kOkWidth  = 90;
static const int kOkHeight = 34;

// Флаг наведения мыши на кнопку (подсветка)
static bool g_hover_ok = false;

// ============================================================
// Хелперы
// ============================================================

// Интернировать атом (получить числовой ID по имени)
static xcb_atom_t InternAtom(const char* name) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(g_conn, 0, (uint16_t)strlen(name), name);
    xcb_intern_atom_reply_t* reply  = xcb_intern_atom_reply(g_conn, cookie, nullptr);
    xcb_atom_t result = reply ? reply->atom : XCB_ATOM_NONE;
    free(reply);
    return result;
}

// Установить заголовок окна (WM_NAME + _NET_WM_NAME/UTF-8)
static void SetTitle(const char* title) {
    xcb_change_property(g_conn, XCB_PROP_MODE_REPLACE, g_window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                        (uint32_t)strlen(title), title);
    xcb_change_property(g_conn, XCB_PROP_MODE_REPLACE, g_window,
                        g_net_wm_name, g_utf8_string, 8,
                        (uint32_t)strlen(title), title);
}

// Читать PNG-байты из памяти (колбэк для cairo PNG stream)
struct PngStream {
    const uint8_t* p;
    const uint8_t* end;
};

static cairo_status_t PngRead(void* closure, unsigned char* data, unsigned int length) {
    auto* s = (PngStream*)closure;
    if (s->p + length > s->end) return CAIRO_STATUS_READ_ERROR;
    memcpy(data, s->p, length);
    s->p += length;
    return CAIRO_STATUS_SUCCESS;
}

// Установить иконку приложения через _NET_WM_ICON (видна в доке/alt-tab)
static void SetAppIcon() {
    PngStream stream{_binary_icons8_error_64_png_start,
                     _binary_icons8_error_64_png_end};

    cairo_surface_t* img = cairo_image_surface_create_from_png_stream(PngRead, &stream);
    if (cairo_surface_status(img) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "[Icon] failed to decode PNG\n");
        cairo_surface_destroy(img);
        return;
    }

    int w = cairo_image_surface_get_width(img);
    int h = cairo_image_surface_get_height(img);

    // Данные _NET_WM_ICON: [width, height, ARGB...]
    // Пиксели 32-битные, порядок AARRGGBB (native endian)
    std::vector<uint32_t> iconData;
    iconData.reserve((size_t)2 + (size_t)w * h);
    iconData.push_back((uint32_t)w);
    iconData.push_back((uint32_t)h);

    // Cairo даёт ARGB32 premultiplied, байты BGRA в памяти.
    // _NET_WM_ICON хочет non-premultiplied ARGB.
    uint32_t* px = (uint32_t*)cairo_image_surface_get_data(img);
    for (int i = 0; i < w * h; i++) {
        uint32_t p = px[i];
        uint8_t a = (p >> 24) & 0xFF;
        uint8_t r = (p >> 16) & 0xFF;
        uint8_t g = (p >> 8)  & 0xFF;
        uint8_t b = (p)       & 0xFF;
        if (a != 0 && a != 255) {   // разделить premultiply
            r = (uint8_t)((r * 255) / a);
            g = (uint8_t)((g * 255) / a);
            b = (uint8_t)((b * 255) / a);
        }
        iconData.push_back(((uint32_t)a << 24) | ((uint32_t)r << 16) |
                           ((uint32_t)g << 8) | (uint32_t)b);
    }
    cairo_surface_destroy(img);

    xcb_atom_t net_wm_icon = InternAtom("_NET_WM_ICON");
    xcb_change_property(g_conn, XCB_PROP_MODE_REPLACE, g_window,
                        net_wm_icon, XCB_ATOM_CARDINAL, 32,
                        (uint32_t)iconData.size(), iconData.data());
    xcb_flush(g_conn);

    printf("[Icon] set %dx%d\n", w, h);
}

// ============================================================
// Отрисовка: фон + текст "Успех" + кнопка "OK"
// ============================================================

static void DrawDialog() {
    xcb_visualtype_t* visual = FindVisual(g_screen);
    if (!visual) {
        fprintf(stderr, "[Draw] visual not found!\n");
        return;
    }
    cairo_surface_t* surface = cairo_xcb_surface_create(g_conn, g_window,
                                                        visual,
                                                        kWindowWidth, kWindowHeight);
    cairo_t* cr = cairo_create(surface);

    // ===== Фон как в GNOME (Adwaita dark: #242424) =====
    cairo_set_source_rgb(cr, 0.141, 0.141, 0.141);   // #242424
    cairo_paint(cr);

    PangoLayout* layout = pango_cairo_create_layout(cr);

    // ===== Текст "Успех" по центру, чуть выше середины =====
    PangoRectangle ink, logical;
    PangoFontDescription* body = pango_font_description_from_string("Liberation Sans 17");
    pango_layout_set_font_description(layout, body);
    pango_layout_set_text(layout, "Успех", -1);

    pango_layout_get_extents(layout, &ink, &logical);
    int tw = logical.width  / PANGO_SCALE;
    int th = logical.height / PANGO_SCALE;
    int tx = (kWindowWidth - tw) / 2;
    int ty = 58;

    cairo_move_to(cr, (double)tx, (double)ty);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    pango_cairo_show_layout(cr, layout);

    // ===== Кнопка "OK" =====
    double x = kOkX, y = kOkY, w = kOkWidth, h = kOkHeight;
    double radius = 8.0;

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + w - radius, y + radius, radius, -G_PI_2, 0);
    cairo_arc(cr, x + w - radius, y + h - radius, radius, 0, G_PI_2);
    cairo_arc(cr, x + radius, y + h - radius, radius, G_PI_2, G_PI);
    cairo_arc(cr, x + radius, y + radius, radius, G_PI, 3 * G_PI_2);
    cairo_close_path(cr);

    // Заливка: GNOME синий (#3584e4), при наведении светлее
    if (g_hover_ok) {
        cairo_set_source_rgb(cr, 0.28, 0.57, 0.92);   // #4890EA при hover
    } else {
        cairo_set_source_rgb(cr, 0.208, 0.518, 0.894); // #3584E4
    }
    cairo_fill_preserve(cr);

    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb(cr, 0.30, 0.30, 0.30);
    cairo_stroke(cr);

    // Текст "OK" по центру кнопки
    PangoFontDescription* btn = pango_font_description_from_string("Liberation Sans Bold 13");
    pango_layout_set_font_description(layout, btn);
    pango_layout_set_text(layout, "OK", -1);
    pango_layout_get_extents(layout, &ink, &logical);
    int bw = logical.width  / PANGO_SCALE;
    int bh = logical.height / PANGO_SCALE;

    cairo_move_to(cr,
                  x + (w - bw) / 2,
                  y + (h - bh) / 2);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    pango_cairo_show_layout(cr, layout);

    pango_font_description_free(body);
    pango_font_description_free(btn);
    g_object_unref(layout);
    FlushDraw(surface);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

// ============================================================
// Обработка событий
// ============================================================

static void HandleEvent(xcb_generic_event_t* event) {
    uint8_t type = event->response_type & ~0x80;

    switch (type) {
        case XCB_EXPOSE: {
            auto* exp = (xcb_expose_event_t*)event;
            if (exp->count == 0) DrawDialog();   // последний из пачки перерисовок
            break;
        }
        case XCB_BUTTON_PRESS: {
            auto* btn = (xcb_button_press_event_t*)event;
            int mx = btn->event_x;
            int my = btn->event_y;
            // Проверка попадания в кнопку OK
            if (mx >= kOkX && mx <= kOkX + kOkWidth &&
                my >= kOkY && my <= kOkY + kOkHeight) {
                printf("[Dialog] OK clicked\n");
                g_running = false;
            }
            break;
        }
        case XCB_MOTION_NOTIFY: {
            auto* mot = (xcb_motion_notify_event_t*)event;
            bool in = (mot->event_x >= kOkX && mot->event_x <= kOkX + kOkWidth &&
                       mot->event_y >= kOkY && mot->event_y <= kOkY + kOkHeight);
            if (in != g_hover_ok) {
                g_hover_ok = in;
                DrawDialog();
            }
            break;
        }
        case XCB_LEAVE_NOTIFY: {
            if (g_hover_ok) {
                g_hover_ok = false;
                DrawDialog();
            }
            break;
        }
        case XCB_CLIENT_MESSAGE: {
            auto* msg = (xcb_client_message_event_t*)event;
            if (msg->data.data32[0] == g_wm_delete) {
                printf("[Dialog] Close button (WM_DELETE_WINDOW)\n");
                g_running = false;
            }
            break;
        }
        default:
            break;
    }
}

// ============================================================
// Создание окна
// ============================================================

static bool CreateDialog() {
    g_conn = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(g_conn)) {
        fprintf(stderr, "Failed to connect to X server\n");
        return false;
    }

    g_screen = xcb_setup_roots_iterator(xcb_get_setup(g_conn)).data;

    g_window = xcb_generate_id(g_conn);

    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t values[] = {
        0x2D2D2D,   // фон
        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_LEAVE_WINDOW |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY
    };

    xcb_create_window(g_conn,
                      XCB_COPY_FROM_PARENT,
                      g_window,
                      g_screen->root,
                      0, 0,
                      kWindowWidth, kWindowHeight,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      g_screen->root_visual,
                      mask, values);

    // Атомы
    g_wm_delete   = InternAtom("WM_DELETE_WINDOW");
    g_net_wm_name = InternAtom("_NET_WM_NAME");
    g_utf8_string = InternAtom("UTF8_STRING");

    xcb_atom_t wm_protocols = InternAtom("WM_PROTOCOLS");

    // Регистрируем протокол закрытия (крестик)
    xcb_change_property(g_conn, XCB_PROP_MODE_REPLACE, g_window,
                        wm_protocols, XCB_ATOM_ATOM, 32, 1, &g_wm_delete);

    // Заголовок окна
    SetTitle("Ошибка");

    // ===== Идентификация приложения для GNOME Shell/Dock =====
    // _NET_WM_PID — PID процесса, чтобы Shell связал окно с приложением
    uint32_t pid = (uint32_t)getpid();
    xcb_atom_t net_wm_pid = InternAtom("_NET_WM_PID");
    xcb_change_property(g_conn, XCB_PROP_MODE_REPLACE, g_window,
                        net_wm_pid, XCB_ATOM_CARDINAL, 32, 1, &pid);

    // WM_CLASS — имя класса окна (должно совпадать с .desktop-файлом).
    // Две NULL-terminated строки: instance\0 class\0
    static const char wmClass[] = "x11dialog\0X11Dialog";
    xcb_change_property(g_conn, XCB_PROP_MODE_REPLACE, g_window,
                        XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8,
                        sizeof(wmClass) - 1, wmClass);

    // Иконка приложения (видна в доке/alt-tab)
    SetAppIcon();

    // Показываем и отправляем команды
    xcb_map_window(g_conn, g_window);
    xcb_flush(g_conn);

    // Принудительная первичная отрисовка (на случай если Expose не придёт)
    DrawDialog();

    g_running = true;
    return true;
}

// ============================================================
// Главный цикл
// ============================================================

static void RunLoop() {
    while (g_running) {
        xcb_generic_event_t* event;
        while ((event = xcb_poll_for_event(g_conn))) {
            HandleEvent(event);
            free(event);
        }
        // Пауза, чтобы не грузить CPU на 100%
        usleep(1000);
    }
}

// ============================================================
// Завершение
// ============================================================

static void DestroyDialog() {
    xcb_destroy_window(g_conn, g_window);
    xcb_disconnect(g_conn);
    g_conn = nullptr;
    g_screen = nullptr;
    g_window = 0;
}

// ============================================================

int main() {
    if (!CreateDialog()) return 1;

    printf("[Dialog] Window open: '%s' [%dx%d]\n", "Ошибка", kWindowWidth, kWindowHeight);

    RunLoop();

    DestroyDialog();
    printf("[Dialog] Done\n");
    return 0;
}
