#include <xcb/xcb.h>

#include "Connection.hpp"


namespace Xcb
{
    class Window
    {
        public:
            Window(const Connection& conn);
            ~Window();

            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;

            Window(Window&& other) noexcept;
            Window& operator=(Window&& other) noexcept;

            xcb_window_t get() const;

        private:
            xcb_connection_t* connection = nullptr;
            xcb_window_t window = 0;

            const xcb_setup_t      *setup  = xcb_get_setup (connection);
        xcb_screen_iterator_t   iter   = xcb_setup_roots_iterator (setup);
        xcb_screen_t           *screen = iter.data;
    };
    
}