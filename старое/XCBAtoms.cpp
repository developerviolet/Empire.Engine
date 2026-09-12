#include "XCBAtoms.hpp"
#include <cstring>
#include <xcb/xcb.h>

void XCBAtoms::init(xcb_connection_t* conn) {
    auto intern = [&](const char* name) {
        return xcb_intern_atom_reply(conn, xcb_intern_atom(conn, 0, strlen(name), name), nullptr)->atom;
    };

    WM_NAME = intern("WM_NAME");
    NET_WM_NAME = intern("_NET_WM_NAME");
    UTF8_STRING = intern("UTF8_STRING");
    WM_PROTOCOLS = intern("WM_PROTOCOLS");
    WM_DELETE_WINDOW = intern("WM_DELETE_WINDOW");
    NET_WM_STATE = intern("_NET_WM_STATE");
    NET_WM_STATE_FULLSCREEN = intern("_NET_WM_STATE_FULLSCREEN");
}