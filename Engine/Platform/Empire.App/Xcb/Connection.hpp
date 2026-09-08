// Connection.hpp
#pragma once

#include <xcb/xcb.h>

namespace Xcb
{
    class Connection
    {
    public:
        Connection();
        ~Connection();

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        Connection(Connection&& other) noexcept;
        Connection& operator=(Connection&& other) noexcept;

        xcb_connection_t* get() const;

       

    private:
        xcb_connection_t* connection = nullptr;
    };
}