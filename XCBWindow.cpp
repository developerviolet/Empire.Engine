//
// Created by devchik10 on 7/8/26.
//

// Подключаем XCB — это библиотека, через которую мы говорим с X сервером
// X сервер — это программа, которая рисует окна в Linux
#include <xcb/xcb.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>   // для sleep() в тесте

// Глобальные переменные, которые хранят состояние окна
// static значит — они видны только внутри этого файла
static xcb_connection_t* g_display = nullptr;
// ^ соединение с X сервером (xcb_connection_t).
//   Через него мы отправляем все команды

static xcb_screen_t*     g_screen  = nullptr;
// ^ экран — содержит информацию о мониторе:
//   какой у него размер, root window, visual и т.д.

static xcb_window_t      g_window  = 0;
// ^ числовой ID нашего окна (xcb_window_t).
//   X сервер обращается к окнам по числу, не по имени

static bool              g_open    = false;
// ^ флаг: открыто окно или нет

static int               g_window_width  = 0;
// ^ текущая ширина окна (обновляется в Create и SetSize)

static int               g_window_height = 0;
// ^ текущая высота окна (обновляется в Create и SetSize)

// Атом WM_DELETE_WINDOW — нужен для проверки в PollEvents
// Сохраняем чтобы каждый раз не делать intern
static xcb_atom_t g_wm_delete = 0;

// Атомы для современного заголовка (_NET_WM_NAME)
// _NET_WM_NAME — свойство с названием окна в UTF-8
// UTF8_STRING — тип данных для UTF-8 строк
// Оба нужны чтобы современные оконные менеджеры (GNOME, KDE)
// видели правильный заголовок, а не только WM_NAME
static xcb_atom_t g_net_wm_name = 0;
static xcb_atom_t g_utf8_string = 0;

// Функция: Создать окно
// Принимает: название, ширину, высоту
// Возвращает: true если всё ок, false если ошибка
bool XCBWindowCreate(const char* title, int width, int height) {
    // ===== ШАГ 1: Соединение с X сервером =====
    //
    // xcb_connect — открывает соединение с X сервером
    // Первый параметр nullptr — берём дисплей из переменной DISPLAY (обычно :0)
    // Второй — сюда можно получить номер экрана, нам не нужен
    printf("[XCB] Connecting to X server...\n");
    g_display = xcb_connect(nullptr, nullptr);

    // Проверяем, соединились ли мы
    // xcb_connection_has_error возвращает 0 если всё хорошо
    if (xcb_connection_has_error(g_display)) {
        printf("[XCB] FAILED to connect to X server\n");
        return false;
    }
    printf("[XCB] Connected successfully\n");

    // ===== ШАГ 2: Получаем screen =====
    //
    // xcb_setup_roots_iterator — получает первый (и обычно единственный) экран
    // Каждый screen содержит:
    //   - root:   корневое окно (рабочий стол)
    //   - root_visual: настройки цвета (RGB, глубина 24)
    g_screen = xcb_setup_roots_iterator(xcb_get_setup(g_display)).data;
    printf("[XCB] Got screen, root = %d\n", g_screen->root);

    // ===== ШАГ 3: Генерируем ID для окна =====
    //
    // xcb_generate_id — просит X сервер дать нам свободный числовой ID
    // Все окна в X имеют уникальный числовой ID
    g_window = xcb_generate_id(g_display);
    printf("[XCB] Generated window ID = %d\n", g_window);

    // Сохраняем размеры окна
    g_window_width  = width;
    g_window_height = height;

    // Маска — говорим что в массиве values будет
    // XCB_CW_BACK_PIXEL — цвет фона
    // XCB_CW_EVENT_MASK — события
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // В массиве сначала фон (синий цвет), потом маска событий
    // Цвет = 0x0000FF = красный 0, зелёный 0, синий 255
    //
    // Цвета в X задаются в формате 0xRRGGBB:
    //   0xFF0000 — красный
    //   0x00FF00 — зелёный
    //   0x0000FF — синий
    //   0x800080 — фиолетовый
    //   0x2D2D2D — тёмно-серый
    uint32_t values[] = {
        0x2D2D2D,                        // фон = тёмно-серый
        XCB_EVENT_MASK_STRUCTURE_NOTIFY   // события
    };

    // XCB_EVENT_MASK_STRUCTURE_NOTIFY — говорит серверу:
    //   "присылай мне события, когда окно меняет размер, закрывается и т.д."
    //   Это НУЖНО чтобы работал крестик (WM_DELETE_WINDOW)
//    uint32_t values[] = { XCB_EVENT_MASK_STRUCTURE_NOTIFY };

    // ===== ШАГ 5: Создаём окно =====
    //
    // xcb_create_window — самая важная функция
    //
    // Параметры по порядку:
    //   1. g_display         — соединение с X сервером
    //   2. XCB_COPY_FROM_PARENT — глубина цвета (копируем от родителя)
    //   3. g_window          — ID нашего нового окна
    //   4. g_screen->root    — родительское окно (корневое = рабочий стол)
    //   5. 0, 0              — X, Y позиция окна
    //   6. width, height     — ширина, высота
    //   7. 0                 — border (толщина рамки, 0 = системная)
    //   8. XCB_WINDOW_CLASS_INPUT_OUTPUT — тип окна (обычное)
    //   9. g_screen->root_visual — цветовой формат (RGB)
    //   10. mask, values     — дополнительные параметры (см. выше)
    xcb_create_window(g_display,
                      XCB_COPY_FROM_PARENT,  // глубина
                      g_window,              // ID окна
                      g_screen->root,         // parent = рабочий стол
                      0, 0,                   // позиция X, Y
                      width, height,          // размер
                      0,                      // border = 0
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,  // класс = обычное окно
                      g_screen->root_visual,  // цветовой формат
                      mask, values);          // доп параметры

    printf("[XCB] Window created\n");

    // ===== ШАГ 6: Устанавливаем заголовок окна =====
    //
    // xcb_change_property — меняет свойство окна
    // Свойства — это данные, прикреплённые к окну (название, размер, и т.д.)
    //
    // Параметры:
    //   1. g_display            — соединение
    //   2. XCB_PROP_MODE_REPLACE — заменить старое значение новым
    //   3. g_window             — наше окно
    //   4. XCB_ATOM_WM_NAME     — что меняем (свойство "имя окна")
    //   5. XCB_ATOM_STRING      — тип данных (строка)
    //   6. 8                    — бит на символ (8 = обычный текст)
    //   7. strlen(title)        — длина строки
    //   8. title                — сама строка
    xcb_change_property(g_display,
                        XCB_PROP_MODE_REPLACE,  // режим = заменить
                        g_window,               // окно
                        XCB_ATOM_WM_NAME,       // свойство = имя окна
                        XCB_ATOM_STRING,        // тип = строка
                        8,                      // 8 бит на символ
                        strlen(title),          // длина
                        title);                 // текст

    // ===== ШАГ 6.5: Устанавливаем современный заголовок =====
    //
    // _NET_WM_NAME — это современный заголовок
    // В отличие от WM_NAME, он поддерживает UTF-8 и спецсимволы
    // Большинство современных WM (GNOME, KDE, i3) используют именно его
    // WM_NAME оставляем для совместимости со старыми WM
    xcb_change_property(g_display,
                        XCB_PROP_MODE_REPLACE,  // заменить
                        g_window,              // окно
                        g_net_wm_name,          // свойство = _NET_WM_NAME
                        g_utf8_string,          // тип = UTF8_STRING
                        8,                      // 8 бит на символ
                        strlen(title),          // длина
                        title);                 // текст

    // ===== ШАГ 7: Получаем атомы =====
    //
    // Атом — это число, которое представляет имя
    // X сервер хранит таблицу: имя → число
    // "WM_PROTOCOLS" → число 177 (например)
    // Мы должны спросить у сервера: "какое число у строки WM_PROTOCOLS?"
    //
    // intern — в переводе "интернировать" (поместить в таблицу)
    auto intern = [](const char* name) -> xcb_atom_t {
        // xcb_intern_atom — отправляет запрос серверу
        // 0 — не создавать атом если его нет (мы хотим найти существующий)
        auto cookie = xcb_intern_atom(g_display, 0, strlen(name), name);
        // xcb_intern_atom_reply — получает ответ на запрос
        auto* reply = xcb_intern_atom_reply(g_display, cookie, nullptr);
        // reply->atom — это число, под которым сервер хранит это имя
        xcb_atom_t result = reply ? reply->atom : 0;
        // reply — malloc'нутый, нужно освободить
        free(reply);
        return result;
    };

    // Получаем числовые ID для наших атомов
    xcb_atom_t wm_protocols = intern("WM_PROTOCOLS");
    xcb_atom_t wm_delete    = intern("WM_DELETE_WINDOW");
    // ^ WM_DELETE_WINDOW — это протокол, который говорит:
    //   "когда пользователь нажимает крестик, пришли событие CLIENT_MESSAGE"
    //   Без этого X сервер просто убьёт окно без предупреждения

    // Сохраняем атом для PollEvents
    g_wm_delete = wm_delete;

    // Интернируем атомы для современного заголовка
    // _NET_WM_NAME — как называется окно (UTF-8)
    // UTF8_STRING — тип UTF-8 строки
    g_net_wm_name = intern("_NET_WM_NAME");
    g_utf8_string = intern("UTF8_STRING");

    printf("[XCB] Atoms: WM_PROTOCOLS=%d, WM_DELETE_WINDOW=%d, _NET_WM_NAME=%d, UTF8_STRING=%d\n",
           wm_protocols, wm_delete, g_net_wm_name, g_utf8_string);

    // ===== ШАГ 8: Говорим серверу про крестик =====
    //
    // Мы говорим: "для окна g_window, в свойство WM_PROTOCOLS,
    // запиши значение WM_DELETE_WINDOW"
    //
    // Теперь когда юзер нажмёт крестик, сервер:
    // 1. Видит, что у окна есть WM_PROTOCOLS
    // 2. Видит, что там записан WM_DELETE_WINDOW
    // 3. Вместо того чтобы убить окно, присылает CLIENT_MESSAGE
    xcb_change_property(g_display,
                        XCB_PROP_MODE_REPLACE,
                        g_window,
                        wm_protocols,          // свойство
                        XCB_ATOM_ATOM,        // тип = атом
                        32,                    // 32 бита на значение
                        1,                     // 1 значение
                        &wm_delete);           // значение

    // ===== ШАГ 9: Показываем окно =====
    //
    // xcb_map_window — делает окно видимым
    // xcb_create_window создало окно, но оно невидимо
    // map_window показывает его на экране
    xcb_map_window(g_display, g_window);
    printf("[XCB] Window mapped (visible)\n");

    // ===== ШАГ 10: Отправляем все команды =====
    //
    // XCB — асинхронный
    // Все команды выше (create_window, change_property, map_window)
    // НЕ были отправлены серверу!
    // Они лежат в буфере
    // xcb_flush — берёт и отправляет всё разом
    xcb_flush(g_display);

    // Отмечаем, что окно открыто
    g_open = true;
    printf("[XCB] Window '%s' created successfully [%dx%d]\n", title, width, height);
    return true;
}

// ===== Функция: Закрыть окно =====
//
// Вызывается когда окно больше не нужно
// Порядок важен:
//   1. Уничтожить окно (xcb_destroy_window)
//   2. Закрыть соединение с сервером (xcb_disconnect)
//   3. Обнулить переменные чтобы случайно не использовать
void XCBWindowDestroy() {
    if (!g_open) return;  // уже закрыто, ничего не делаем

    printf("[XCB] Destroying window...\n");

    // xcb_destroy_window — говорит серверу "уберй окно с экрана и забудь про него"
    // После этого g_window становится недействительным
    xcb_destroy_window(g_display, g_window);

    // xcb_disconnect — закрывает соединение с X сервером
    // Открывать новое соединение для нового окна надо будет заново
    xcb_disconnect(g_display);

    // Обнуляем переменные — чтобы случайно не использовать старое окно
    g_display = nullptr;
    g_screen  = nullptr;
    g_window  = 0;
    g_open    = false;


    printf("[XCB] Window destroyed\n");
}

// ===== Функция: Изменить заголовок окна =====
//
// Меняет оба свойства:
//   WM_NAME — старый способ (Latin-1, ASCII)
//   _NET_WM_NAME — современный способ (UTF-8)
//
// Это гарантирует что заголовок обновится в любом WM
void XCBWindowSetTitle(const char* title) {
    printf("[XCB] SetTitle: '%s'\n", title);

    // Старый заголовок — WM_NAME (ASCII/ Latin-1)
    xcb_change_property(g_display, XCB_PROP_MODE_REPLACE, g_window,
                        XCB_ATOM_WM_NAME,    // свойство
                        XCB_ATOM_STRING,      // тип = строка
                        8,                    // 8 бит на символ
                        strlen(title),        // длина
                        title);

    // Современный заголовок — _NET_WM_NAME (UTF-8)
    xcb_change_property(g_display, XCB_PROP_MODE_REPLACE, g_window,
                        g_net_wm_name,        // свойство = _NET_WM_NAME
                        g_utf8_string,         // тип = UTF8_STRING
                        8,                    // 8 бит на символ
                        strlen(title),        // длина
                        title);

    // Отправляем команды серверу
    xcb_flush(g_display);
}

// ===== Функция: Изменить размер окна =====
//
// Меняет ширину и высоту окна через xcb_configure_window
// configure_window может менять много параметров (позицию, размер, border)
// Мы используем только XCB_CONFIG_WINDOW_WIDTH и XCB_CONFIG_WINDOW_HEIGHT
void XCBWindowSetSize(int width, int height) {
    // Массив значений для configure_window
    // Порядок соответствует маске:
    //   XCB_CONFIG_WINDOW_WIDTH  = первое значение
    //   XCB_CONFIG_WINDOW_HEIGHT = второе значение
    uint32_t values[] = { (uint32_t)width, (uint32_t)height };

    // Маска — говорим серверу какие параметры меняем
    // XCB_CONFIG_WINDOW_WIDTH  (бит 2) — ширина
    // XCB_CONFIG_WINDOW_HEIGHT (бит 3) — высота
    uint32_t mask = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;

    // xcb_configure_window — изменение параметров окна
    xcb_configure_window(g_display, g_window, mask, values);

    // Обновляем сохранённые размеры
    g_window_width  = width;
    g_window_height = height;

    // Отправляем команду серверу
    xcb_flush(g_display);

    printf("[XCB] SetSize: %dx%d\n", width, height);
}

// ===== Функция: Обработать события окна =====
//
// Проверяет есть ли новые события от X сервера
// Если событие — крестик (WM_DELETE_WINDOW), ставит g_open = false
//
// Должна вызываться каждый кадр в главном цикле
void XCBWindowPollEvents() {
    // xcb_poll_for_event — проверяет очередь событий
    // Если есть событие — возвращает указатель на него
    // Если нет — возвращает nullptr (цикл не блокируется)
    xcb_generic_event_t* event;
    while ((event = xcb_poll_for_event(g_display))) {
        // response_type — тип события
        // & ~0x80 — убираем флаг "событие отправлено" (xcb internal)
        uint8_t type = event->response_type & ~0x80;

        // Проверяем: это CLIENT_MESSAGE?
        // CLIENT_MESSAGE — это событие, которое сервер шлёт когда
        // окно должно выполнить протокол (например, закрыться)
        if (type == XCB_CLIENT_MESSAGE) {
            // Превращаем общее событие в CLIENT_MESSAGE событие
            auto* msg = (xcb_client_message_event_t*)event;

            // Проверяем: это именно WM_DELETE_WINDOW?
            // data.data32[0] — первый 4-байтовый кусок данных
            // Содержит атом, который мы записали в WM_PROTOCOLS
            if (msg->data.data32[0] == g_wm_delete) {
                printf("[XCB] WM_DELETE_WINDOW (close button clicked)\n");
                g_open = false;
            }
        }

        // Важно: освобождаем память события
        // xcb_poll_for_event выделяет память под каждое событие
        // если не освободить — утечка памяти
        free(event);
    }
}

// ============================================================
// ВРЕМЕННЫЙ ТЕСТ (потом перенесём в Tests/)
// ============================================================
int main() {
    if (!XCBWindowCreate("Empire Engine", 800, 600)) return 1;

    printf("[TEST] Window is open. Title will change in 2 seconds.\n");

    sleep(2);
    XCBWindowSetTitle("Новый заголовок!!!");

    printf("[TEST] Title changed. Size will change in 2 seconds.\n");

    sleep(2);
    XCBWindowSetSize(400, 300);

    printf("[TEST] Size changed. Close via close button.\n");

    while (g_open) {
        XCBWindowPollEvents();
    }

    XCBWindowDestroy();
    printf("[TEST] Done\n");
    return 0;
}