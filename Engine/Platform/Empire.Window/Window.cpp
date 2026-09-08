#include "Window.hpp"

#include <memory>

#include "Xcb/Window.hpp"

namespace Engine
{
    struct Window::Impl
    {
        std::unique_ptr<Xcb::Window> window;

        Impl(const WindowConfig& config)
            : window(std::make_unique<Xcb::Window>(config))
        {
        }
    };

    Window::Window(const WindowConfig& config)
    {
        impl = new Impl(config);
    }

    Window::~Window()
    {
        delete impl;
    }

    bool Window::PollEvent()
    {
        if (!impl->window)
            return false;

        return impl->window->PollEvent();
    }

    bool Window::ShouldClose() const
    {
        if (!impl->window)
            return true;

        return impl->window->ShouldClose();
    }

    void Window::Close()
    {
        impl->window.reset();
    }

    bool Window::IsOpen() const
    {
        return impl->window != nullptr;
    }
}