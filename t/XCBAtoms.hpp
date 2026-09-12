#pragma once
#include <xcb/xcb.h>

struct XCBAtoms {
    xcb_atom_t WM_NAME = 0;
    xcb_atom_t NET_WM_NAME = 0;
    xcb_atom_t UTF8_STRING = 0;
    xcb_atom_t WM_PROTOCOLS = 0;
    xcb_atom_t WM_DELETE_WINDOW = 0;
    xcb_atom_t NET_WM_STATE = 0;
    xcb_atom_t NET_WM_STATE_FULLSCREEN = 0;

    // Инициализирует все атомы (вызывать один раз после connect)
    void init(xcb_connection_t* conn);
};