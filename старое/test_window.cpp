#include "Engine/Platform/Window/X11/XCBHelpers.hpp"
#include <unistd.h>

int main() {
    XCBNativeHandles handles;
    initWindow(handles, "Empire Engine", 1280, 720, false, 100, 100);

    if (!handles.connection) return 1;

    while (true) {
        pollWindowEvents(handles);
        if (!handles.connection) break;
        usleep(16000);
    }

    return 0;
}
