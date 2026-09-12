//
// Created by devchik10 on 7/7/26.
//

#pragma once
#include <string>
#include <xcb/xcb.h>

struct XCBNativeHandles {
    xcb_connection_t* connection = nullptr;
    xcb_screen_t* screen = nullptr;
    xcb_window_t handle = 0;
};

bool createXCBWindow(
    XCBNativeHandles& out,
    const std::string& title,
    int width, int height,
    bool fullscreen,
    int x, int y
    );

void destroyXCBWindow(XCBNativeHandles& handle);
void setXCBWindowTitle(XCBNativeHandles& handle, const std::string& title);
void setXCBWindowSize(XCBNativeHandles& handle, int width, int height);
void setXCBWindowFullscreen(XCBNativeHandles& handle, bool fullscreen);
void setXCBWindowPosition(XCBNativeHandles& handle, int x, int y);

