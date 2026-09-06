#include <xcb/xcb.h>

#include "Aliases.hpp"
#include "Window.hpp"
#include "Connection.hpp"

// Window.cpp

using namespace Engine;

namespace Xcb
{
    Window::Window(Connection& connection)
    : connection(connection)
    {
        i32 width = 150;
        i32 height = 150;

        window = xcb_generate_id(connection.get());
        const xcb_setup_t* setup = xcb_get_setup(connection.get());
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);

        xcb_screen_t* screen = iter.data;

        xcb_create_window (connection.get(),                    /* Connection          */
                           XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
                           window,                        /* window Id           */
                           screen->root,                  /* parent window       */
                           0, 0,                          /* x, y                */
                           width, height,                      /* width, height       */
                           10,                            /* border_width        */
                           XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                           screen->root_visual,           /* visual              */
                           0, NULL );                     /* masks, not used yet */

                           xcb_map_window(connection.get(), window);
                           xcb_flush(connection.get());
    }

    Window::~Window()
    {
        if (connection.get())
        {
            xcb_destroy_window(connection.get(), window);
            xcb_flush(connection.get());
        }
        
        
    }
}