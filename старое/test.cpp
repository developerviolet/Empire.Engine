//
// Created by devchik10 on 7/8/26.
//

#include "../Engine/Platform/X11/XCBWindowAPI.hpp"
#include <unistd.h>

int main() {
    XCBNativeHandles handles;
    if (!createXCBWindow(handles, "Empire Engine", 1280, 720, false, 0, 0)) return 1;

    // Простой цикл ожидания (пока без событий)
    while (true) {
        usleep(16000);  // ~60 FPS заглушка
    }

    destroyXCBWindow(handles);
    return 0;
}