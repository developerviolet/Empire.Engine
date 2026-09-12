#pragma once
#include <xcb/xcb.h>

struct XCBNativeHandles {
    xcb_connection_t* connection = nullptr;
    xcb_screen_t*     screen     = nullptr;
    xcb_window_t      handle     = 0;
};