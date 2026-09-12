//
// Created by devchik10 on 5/16/26.
//

#include <iostream>
#include <string>
#include <format> // Фича C++20/23 для красивого вывода строк

struct Window {
    std::string title = "DEvchik10 Engine";
    int width = 800;
    int height = 600;
    bool isOpen = false;

    void create() {
        std::cout << std::format("[C++ Window]: '{}' [{}x{}]\n", title, width, height);
        isOpen = true;
    }
};

Window window;
int frameCount = 0;

void Init() {
    std::cout << "[C++ Engine]: Initializing..\n";
    window.create();
    std::cout << "[C++ Engine]: Готово!\n\n";
}

void Update(double deltaTime) {
    frameCount++;

    std::cout << std::format("[Engine Loop]: Кадр: {} | DeltaTime: {} сек\n", frameCount, deltaTime);

    if (frameCount >= 5) {
        window.isOpen = false;
        std::cout << "\n[Engine]: Симуляция закрытия окна... \n";
    }
}

int main() {
    Init();

    double currentDeltaTime = 0.016;

    while (window.isOpen) {
        Update(currentDeltaTime);
    }
    std::cout << "[C++ Engine]: Работа завершена.\n";
    return 0;
}
