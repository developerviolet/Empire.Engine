#include <xcb/xcb.h>

#include "Connection.hpp"

namespace Xcb
{
    class Window
    {
        public:
            Window(Connection& conn);
            ~Window();
            
            xcb_window_t get() const;
            

        private:
            Connection& connection;

            xcb_window_t window = 0;
        
    };
    
}