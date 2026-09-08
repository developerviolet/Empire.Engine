#pragma once

#include "WindowConfig.hpp"

namespace Engine
{
    class Window
    {
    public:
        Window(const WindowConfig& config);
        ~Window();

        bool PollEvent();
        bool ShouldClose() const;
        void Close();
        bool IsOpen() const;

    private:
        struct Impl;
        Impl* impl = nullptr;
    };
}