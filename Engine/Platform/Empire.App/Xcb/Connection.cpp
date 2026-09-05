// Connection.cpp
#include "Connection.hpp"
#include "../../../Aliases.hpp"

using namespace Engine;

namespace Platform
{
    Connection::Connection()
        : connection(xcb_connect(nullptr, nullptr))
    {
    }

    Connection::~Connection()
    {
        if (connection)
            xcb_disconnect(connection);
    }

    Connection::Connection(Connection&& other) noexcept
        : connection(other.connection)
    {
        other.connection = nullptr;
    }

    Connection& Connection::operator=(Connection&& other) noexcept
    {
        if (this != &other)
        {
            if (connection)
                xcb_disconnect(connection);

            connection = other.connection;
            other.connection = nullptr;
        }

        return *this;
    }

    xcb_connection_t* Connection::get() const
    {
        return connection;
    }
}