#include <thread>
#include <chrono>

#include "Window.hpp"
#include "WindowConfig.hpp"

using namespace Engine;

int main()
{
    WindowConfig editorConfig;
    editorConfig.width = 800;
    editorConfig.height = 600;

    WindowConfig gameConfig;
    gameConfig.width = 800;
    gameConfig.height = 600;

    Window editor(editorConfig);
    Window game(gameConfig);

    while (editor.IsOpen() || game.IsOpen())
    {
        if (editor.IsOpen())
        {
            editor.PollEvent();

            if (editor.ShouldClose())
                editor.Close();
        }

        if (game.IsOpen())
        {
            game.PollEvent();

            if (game.ShouldClose())
                game.Close();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}