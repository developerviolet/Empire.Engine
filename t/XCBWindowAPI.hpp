#// Engine/Platform/X11/XCBWindowAPI.hpp
#pragma once
#include "XCBNativeHandles.hpp"
#include <string>

bool createXCBWindow(
    XCBNativeHandles& out,
    const std::string& title,
    int width, int height,
    bool fullscreen,
    int x, int y
);

void destroyXCBWindow(XCBNativeHandles& h);
void setXCBWindowTitle(XCBNativeHandles& h, const std::string& title);
void setXCBWindowSize(XCBNativeHandles& h, int w, int h_);
void setXCBWindowPosition(XCBNativeHandles& h, int x, int y);
void setXCBWindowFullscreen(XCBNativeHandles& h, bool fs);