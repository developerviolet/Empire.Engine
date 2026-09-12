#include "XCBNativeHandles.hpp"
#include "XCBAtoms.hpp"

#include <iostream>
#include <cstring>
#include <xcb/xcb.h>

static XCBAtoms g_atoms;

// ============================================================
// Создание окна
// ============================================================
bool createXCBWindow(
    XCBNativeHandles& out,
    const std::string& title,
    int width, int height,
    bool fullscreen,
    int x, int y)
{
    out.connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(out.connection)) {
        std::cout << "[XCB]: Не удалось подключиться к X серверу\n";
        return false;
    }

    out.screen = xcb_setup_roots_iterator(xcb_get_setup(out.connection)).data;
    out.handle = xcb_generate_id(out.connection);

    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t values[] = { 0x800080, XCB_EVENT_MASK_EXPOSURE };

    xcb_create_window(out.connection, XCB_COPY_FROM_PARENT, out.handle,
        out.screen->root, x, y, width, height, 0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, out.screen->root_visual,
        mask, values);

    // WM_NAME (Latin-1)
    xcb_change_property(out.connection, XCB_PROP_MODE_REPLACE, out.handle,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
        title.size(), title.data());

    // _NET_WM_NAME (UTF-8)
    if (!g_atoms.NET_WM_NAME) g_atoms.init(out.connection);
    xcb_change_property(out.connection, XCB_PROP_MODE_REPLACE, out.handle,
        g_atoms.NET_WM_NAME, g_atoms.UTF8_STRING, 8,
        title.size(), title.data());

    // WM_DELETE_WINDOW (крестик)
    xcb_change_property(out.connection, XCB_PROP_MODE_REPLACE, out.handle,
        g_atoms.WM_PROTOCOLS, XCB_ATOM_ATOM, 32,
        1, &g_atoms.WM_DELETE_WINDOW);

    // Fullscreen
    if (fullscreen) {
        xcb_client_message_event_t event = {};
        event.response_type = XCB_CLIENT_MESSAGE;
        event.format = 32;
        event.window = out.handle;
        event.type = g_atoms.NET_WM_STATE;
        event.data.data32[0] = 1;
        event.data.data32[1] = g_atoms.NET_WM_STATE_FULLSCREEN;
        event.data.data32[2] = 0;

        xcb_send_event(out.connection, 0, out.screen->root,
            XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
            (const char*)&event);
    }

    xcb_map_window(out.connection, out.handle);
    xcb_flush(out.connection);
    return true;
}

// ============================================================
// Остальные функции
// ============================================================
void destroyXCBWindow(XCBNativeHandles& h) {
    if (h.connection) {
        xcb_destroy_window(h.connection, h.handle);
        xcb_disconnect(h.connection);
        h.connection = nullptr;
    }
}

void setXCBWindowTitle(XCBNativeHandles& h, const std::string& t) {
    xcb_change_property(h.connection, XCB_PROP_MODE_REPLACE, h.handle,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, t.size(), t.data());
    if (g_atoms.NET_WM_NAME) {
        xcb_change_property(h.connection, XCB_PROP_MODE_REPLACE, h.handle,
            g_atoms.NET_WM_NAME, g_atoms.UTF8_STRING, 8, t.size(), t.data());
    }
    xcb_flush(h.connection);
}

void setXCBWindowSize(XCBNativeHandles& h, int w, int h_) {
    uint32_t vals[] = { (uint32_t)w, (uint32_t)h_ };
    xcb_configure_window(h.connection, h.handle,
        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, vals);
    xcb_flush(h.connection);
}

void setXCBWindowPosition(XCBNativeHandles& h, int x, int y) {
    uint32_t vals[] = { (uint32_t)x, (uint32_t)y };
    xcb_configure_window(h.connection, h.handle,
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, vals);
    xcb_flush(h.connection);
}

void setXCBWindowFullscreen(XCBNativeHandles& h, bool fs) {
    if (!g_atoms.NET_WM_STATE) g_atoms.init(h.connection);
    xcb_client_message_event_t event = {};
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.window = h.handle;
    event.type = g_atoms.NET_WM_STATE;
    event.data.data32[0] = fs ? 1 : 0;
    event.data.data32[1] = g_atoms.NET_WM_STATE_FULLSCREEN;
    event.data.data32[2] = 0;

    xcb_send_event(h.connection, 0, h.screen->root,
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
        (const char*)&event);
    xcb_flush(h.connection);
}