#pragma once

#include <xcb/xcb.h>

#include "WindowConfig.hpp"

namespace Xcb
{
    class Window
    {
    public:
        Window(const Engine::WindowConfig& config);
        ~Window();

        xcb_window_t Get() const;

        bool PollEvent();
        bool ShouldClose() const;

    private:
        bool HandleEvent(const xcb_generic_event_t* event);

        xcb_window_t window = 0;

        xcb_atom_t protocolsAtom = 0;
        xcb_atom_t deleteAtom = 0;

        bool closeRequested = false;
    };
}