#include "Window.hpp"

#include <cstdlib>
#include <xcb/xcb.h>

#include "Empire.App/Xcb/Connection.hpp"
#include "Empire.App/Xcb/EventQueue.hpp"
#include "WindowConfig.hpp"

namespace Xcb
{
    Window::Window(const Engine::WindowConfig& config)
    {
        xcb_connection_t* conn = Connection::Instance().get();

        window = xcb_generate_id(conn);

        const xcb_setup_t* setup = xcb_get_setup(conn);
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
        xcb_screen_t* screen = iter.data;

        xcb_create_window(conn,
                           XCB_COPY_FROM_PARENT,
                           window,
                           screen->root,
                           0, 0,
                           config.width, config.height,
                           10,
                           XCB_WINDOW_CLASS_INPUT_OUTPUT,
                           screen->root_visual,
                           0, nullptr);

        // WM_DELETE_WINDOW setup — чтобы получать событие "юзер запросил закрытие"
        // вместо разрыва соединения при закрытии окна
        xcb_intern_atom_cookie_t protocolsCookie =
            xcb_intern_atom(conn, 0, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* protocolsReply =
            xcb_intern_atom_reply(conn, protocolsCookie, nullptr);

        xcb_intern_atom_cookie_t deleteCookie =
            xcb_intern_atom(conn, 0, 16, "WM_DELETE_WINDOW");
        xcb_intern_atom_reply_t* deleteReply =
            xcb_intern_atom_reply(conn, deleteCookie, nullptr);

        if (protocolsReply && deleteReply)
        {
            protocolsAtom = protocolsReply->atom;
            deleteAtom = deleteReply->atom;

            xcb_change_property(conn,
                                 XCB_PROP_MODE_REPLACE,
                                 window,
                                 protocolsAtom,
                                 XCB_ATOM_ATOM,
                                 32, 1,
                                 &deleteAtom);
        }

        std::free(protocolsReply);
        std::free(deleteReply);

        EventQueue::Instance().RegisterWindow(window);

        xcb_map_window(conn, window);
        xcb_flush(conn);
    }

    Window::~Window()
    {
        xcb_connection_t* conn = Connection::Instance().get();

        EventQueue::Instance().UnregisterWindow(window);

        if (window != 0)
        {
            xcb_destroy_window(conn, window);
            xcb_flush(conn);
        }
    }

    xcb_window_t Window::Get() const
    {
        return window;
    }

    bool Window::HandleEvent(const xcb_generic_event_t* event)
    {
        if (!event)
            return false;

        const uint8_t responseType = event->response_type & ~0x80;

        if (responseType != XCB_CLIENT_MESSAGE)
            return false;

        const auto* clientMessage =
            reinterpret_cast<const xcb_client_message_event_t*>(event);

        if (clientMessage->window != window)
            return false;

        if (clientMessage->type != protocolsAtom)
            return false;

        if (clientMessage->data.data32[0] != deleteAtom)
            return false;

std::cout << "Close event for window: " << window 
          << ", received on window: " << clientMessage->window << '\n';

        closeRequested = true;
        return true;
    }

    bool Window::PollEvent()
    {
        xcb_generic_event_t* event = EventQueue::Instance().PollEvent(window);

        if (!event)
            return false;

        const bool handled = HandleEvent(event);
        std::free(event);

        return handled;
    }

    bool Window::ShouldClose() const
    {
        return closeRequested;
    }
}