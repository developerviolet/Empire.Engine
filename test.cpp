

#include <iostream>
#include <string>
#include <format>
#include <cstring>
#include <xcb/xcb.h>

// ============================================================
// Window — основная структура окна
// ============================================================
struct Window {
    // --- Публичные поля (доступны из любой части кода) ---
    std::string title = "DEvchik10 Engine";  // Заголовок окна
    int width = 800;                         // Ширина окна в пикселях
    int height = 600;                        // Высота окна в пикселях
    bool fullscreen = false;                 // true = полноэкранный режим
    bool isOpen = false;                     // Флаг: открыто ли окно?

    // --- Приватные по смыслу поля (XCB-специфичные) ---
    xcb_connection_t* connection = nullptr;  // Указатель на соединение с X Server
    xcb_screen_t*     screen     = nullptr;  // Указатель на экран (монитор)
    xcb_window_t      handle     = 0;        // Числовой ID нашего окна в X11

    // ============================================================
    // create() — создаёт реальное окно через XCB
    // ============================================================
    void create() {
        // 1. Подключаемся к X серверу
        connection = xcb_connect(nullptr, nullptr);

        // 2. Проверяем, удалось ли подключиться
        if (xcb_connection_has_error(connection)) {
            std::cout << "[XCB]: Не удалось подключиться к X серверу\n";
            return;
        }

        // 3. Получаем первый экран
        screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

        // 4. Генерируем уникальный ID для будущего окна
        handle = xcb_generate_id(connection);

        // 5. Создаём окно (пока невидимое)
        //    Задаём атрибуты: цвет фона (фиолетовый) + маска событий
        uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t values[] = {
            0x800080,                                  // фиолетовый фон (RRGGBB)
            XCB_EVENT_MASK_EXPOSURE                    // чтобы приходили XCB_EXPOSE
        };
        xcb_create_window(
            connection, XCB_COPY_FROM_PARENT, handle, screen->root,
            0, 0, width, height, 0,
            XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
            mask, values
        );

        // ============================================================
        // Устанавливаем заголовок окна (WM_NAME — старый стандарт)
        // ============================================================
        // XCB_ATOM_WM_NAME   — стандартный атом для заголовка
        // XCB_ATOM_STRING    — тип данных: обычная строка (Latin-1)
        // 8                  — 8 бит на символ
        // title.size()       — длина строки в байтах
        // title.data()       — данные строки
        xcb_change_property(
            connection, XCB_PROP_MODE_REPLACE, handle,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
            title.size(), title.data()
        );

        // ============================================================
        // Устанавливаем заголовок в UTF-8 (_NET_WM_NAME — современный стандарт)
        // ============================================================
        // _NET_WM_NAME поддерживает юникод (русский, японский и т.д.)
        // В отличие от WM_NAME, который работает только с Latin-1
        // Атом _NET_WM_NAME надо получить через xcb_intern_atom
        xcb_intern_atom_cookie_t nameCookie = xcb_intern_atom(
            connection, 0, 12, "_NET_WM_NAME"
        );
        xcb_intern_atom_reply_t* nameReply = xcb_intern_atom_reply(
            connection, nameCookie, nullptr
        );

        xcb_intern_atom_cookie_t utf8Cookie = xcb_intern_atom(
            connection, 0, 11, "UTF8_STRING"
        );
        xcb_intern_atom_reply_t* utf8Reply = xcb_intern_atom_reply(
            connection, utf8Cookie, nullptr
        );

        if (nameReply && utf8Reply) {
            xcb_change_property(
                connection, XCB_PROP_MODE_REPLACE, handle,
                nameReply->atom,
                utf8Reply->atom,                // правильный тип — UTF8_STRING
                8,
                title.size(), title.data()
            );
        }

        free(nameReply);
        free(utf8Reply);

        // ============================================================
        // Регистрируем атом WM_DELETE_WINDOW (чтобы крестик работал)
        // ============================================================
        // WM_PROTOCOLS     — список поддерживаемых протоколов окном
        // WM_DELETE_WINDOW — протокол "закрыть окно"
        xcb_intern_atom_cookie_t protocolsCookie = xcb_intern_atom(
            connection, 0, 12, "WM_PROTOCOLS"
        );
        xcb_intern_atom_cookie_t deleteCookie = xcb_intern_atom(
            connection, 0, 16, "WM_DELETE_WINDOW"
        );

        xcb_intern_atom_reply_t* protocolsReply = xcb_intern_atom_reply(
            connection, protocolsCookie, nullptr
        );
        xcb_intern_atom_reply_t* deleteReply = xcb_intern_atom_reply(
            connection, deleteCookie, nullptr
        );

        if (protocolsReply && deleteReply) {
            xcb_change_property(
                connection, XCB_PROP_MODE_REPLACE, handle,
                protocolsReply->atom, XCB_ATOM_ATOM, 32,
                1, &deleteReply->atom
            );
        }

        free(protocolsReply);
        free(deleteReply);

        // ============================================================
        // Если fullscreen = true — переводим в полноэкранный режим
        // ============================================================
        if (fullscreen) {
            // Получаем атом _NET_WM_STATE (протокол EWMH)
            xcb_intern_atom_cookie_t stateCookie = xcb_intern_atom(
                connection, 0, 12, "_NET_WM_STATE"
            );
            // Получаем атом _NET_WM_STATE_FULLSCREEN
            xcb_intern_atom_cookie_t fsCookie = xcb_intern_atom(
                connection, 0, 24, "_NET_WM_STATE_FULLSCREEN"
            );

            xcb_intern_atom_reply_t* stateReply = xcb_intern_atom_reply(
                connection, stateCookie, nullptr
            );
            xcb_intern_atom_reply_t* fsReply = xcb_intern_atom_reply(
                connection, fsCookie, nullptr
            );

            if (stateReply && fsReply) {
                // Отправляем ClientMessage корневому окну
                // Это стандартный способ попросить WM перевести окно в fullscreen
                xcb_client_message_event_t event;
                memset(&event, 0, sizeof(event));
                event.response_type  = XCB_CLIENT_MESSAGE;
                event.format         = 32;
                event.window         = handle;
                event.type           = stateReply->atom;
                event.data.data32[0] = 1;          // _NET_WM_STATE_ADD
                event.data.data32[1] = fsReply->atom;  // _NET_WM_STATE_FULLSCREEN
                event.data.data32[2] = 0;          // второй атом не используется

                // Отправляем событие корневому окну
                xcb_send_event(
                    connection, 0, screen->root,
                    XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                    XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                    (const char*)&event
                );
            }

            free(stateReply);
            free(fsReply);
        }

        // 6. Делаем окно видимым
        xcb_map_window(connection, handle);

        // 7. Отправляем все команды на X сервер
        xcb_flush(connection);

        // 8. Заливаем окно фиолетовым при создании
        fillColor(128, 0, 128);

        // 9. Отмечаем, что окно открыто
        isOpen = true;
        std::cout << std::format("[XCB Window]: '{}' [{}x{}]\n", title, width, height);
    }

    // ============================================================
    // fillColor() — заливает окно сплошным цветом через XCB
    //    Внимание: это X11-специфичный код.
    //    В будущем движке рендеринг будет через Vulkan/DiligentCore.
    //    Здесь — просто для наглядности, чтобы окно не было пустым.
    // ============================================================
    void fillColor(uint8_t r, uint8_t g, uint8_t b) {
        // 1. Создаём графический контекст (GC) — набор параметров рисования
        xcb_gcontext_t gc = xcb_generate_id(connection);

        // Формируем 32-битный цвет: 0xRRGGBB
        uint32_t color = (r << 16) | (g << 8) | b;
        uint32_t mask = XCB_GC_FOREGROUND;  // меняем только цвет переднего плана

        xcb_create_gc(connection, gc, handle, mask, &color);

        // 2. Рисуем залитый прямоугольник на всё окно
        xcb_rectangle_t rect = { 0, 0, (uint16_t)width, (uint16_t)height };
        xcb_poly_fill_rectangle(connection, handle, gc, 1, &rect);

        // 3. Отправляем команды
        xcb_flush(connection);

        // 4. Освобождаем GC (он нам больше не нужен для этого кадра)
        xcb_free_gc(connection, gc);
    }

    // ============================================================
    // waitForEvents() — ждёт и обрабатывает события от X сервера
    //    БЛОКИРУЮЩИЙ режим — 0% CPU в простое
    // ============================================================
    void waitForEvents() {
        xcb_generic_event_t* event;  // Указатель на событие

        // xcb_wait_for_event блокирует поток до прихода события
        while ((event = xcb_wait_for_event(connection))) {
            // response_type — тип события
            // & ~0x80 — маскируем флаг "сгенерировано сервером"
            uint8_t type = event->response_type & ~0x80;

            // XCB_CLIENT_MESSAGE — сообщение от оконного менеджера
            if (type == XCB_CLIENT_MESSAGE) {
                isOpen = false;  // закрываем окно
                free(event);
                break;
            }

            // XCB_EXPOSE — окно стало видимым или его часть нужно перерисовать
            if (type == XCB_EXPOSE) {
                // Перерисовываем окно (заливаем цветом)
                fillColor(128, 0, 128);  // фиолетовый
            }

            // TODO: позже — клавиатура, мышь, resize

            free(event);
        }
    }

    // ============================================================
    // destroy() — корректно закрывает окно и соединение
    // ============================================================
    void destroy() {
        if (connection) {
            xcb_destroy_window(connection, handle);
            xcb_disconnect(connection);
            connection = nullptr;
        }
    }
};

// ============================================================
// Глобальные переменные
// ============================================================
Window window;

// ============================================================
// Init() — инициализация движка
// ============================================================
void Init() {
    std::cout << "[C++ Engine]: Initializing..\n";
    window.create();
    std::cout << "[C++ Engine]: Готово!\n\n";
}

// ============================================================
// main() — точка входа
// ============================================================
int main() {
    Init();

    // Главный цикл: 0% CPU, просыпается только на события
    while (window.isOpen) {
        window.waitForEvents();
    }

    window.destroy();
    std::cout << "[C++ Engine]: Работа завершена.\n";
    return 0;
}


// g++ -std=c++20 test.cpp -lxcb -o test && ./test
