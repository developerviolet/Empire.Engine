#include <xcb/xcb.h>
#include "Window.hpp"
#include <iostream>

#include "Connection.hpp"
// Window.cpp

namespace Xcb
{
    Window::Window(const Connection& conn)
        : connection(conn.get())
    {
        window = xcb_generate_id(connection);


         xcb_create_window (connection,                    /* Connection          */
                           XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
                           window,                        /* window Id           */
                           screen->root,                  /* parent window       */
                           0, 0,                          /* x, y                */
                           150, 150,                      /* width, height       */
                           10,                            /* border_width        */
                           XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                           screen->root_visual,           /* visual              */
                           0, NULL );                     /* masks, not used yet */

                           xcb_map_window(connection, window);
xcb_flush(connection);

    }
    

    Window::~Window()
    {
        // пока пусто, добавим xcb_destroy_window позже
        xcb_destroy_window(connection, window);
        xcb_flush(connection);
        std::cout << "Window destroyed\n";
    }

    Window::Window(Window&& other) noexcept
        : connection(other.connection), window(other.window)
    {
        other.connection = nullptr;
        other.window = 0;
    }

    Window& Window::operator=(Window&& other) noexcept
    {
        if (this != &other)
        {
            connection = other.connection;
            window = other.window;
            other.connection = nullptr;
            other.window = 0;
        }
        return *this;
    }

    xcb_window_t Window::get() const
    {
        return window;
    }
}