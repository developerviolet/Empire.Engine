#include "Window.hpp"

#include <cstdlib>
#include <xcb/xcb.h>

#include "Empire.App/Xcb/Connection.hpp"
#include "Empire.App/Xcb/EventQueue.hpp"
#include "WindowConfig.hpp"
#include "Aliases.hpp"

using namespace Engine;

namespace Xcb
{
    namespace
    {
        void SendNetWmStateMessage(
            xcb_connection_t* conn,
            xcb_window_t window,
            xcb_screen_t* screen,
            i32 action,
            xcb_atom_t state1,
            xcb_atom_t state2 = 0)
        {
            xcb_intern_atom_cookie_t stateCookie =
                xcb_intern_atom(conn, 0, 13, "_NET_WM_STATE");

            xcb_intern_atom_reply_t* stateReply =
                xcb_intern_atom_reply(conn, stateCookie, nullptr);

            if (!stateReply)
                return;

            xcb_client_message_event_t event = {};

            event.response_type = XCB_CLIENT_MESSAGE;
            event.format = 32;
            event.window = window;
            event.type = stateReply->atom;

            event.data.data32[0] = action;
            event.data.data32[1] = state1;
            event.data.data32[2] = state2;
            event.data.data32[3] = 1;

            xcb_send_event(
                conn,
                0,
                screen->root,
                XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                reinterpret_cast<const char*>(&event));

            std::free(stateReply);

            xcb_flush(conn);
        }

        b8 NetWmStateContains(
            xcb_connection_t* conn,
            xcb_window_t window,
            xcb_atom_t targetAtom)
        {
            xcb_intern_atom_cookie_t stateCookie =
                xcb_intern_atom(conn, 0, 13, "_NET_WM_STATE");

            xcb_intern_atom_reply_t* stateReply =
                xcb_intern_atom_reply(conn, stateCookie, nullptr);

            if (!stateReply)
                return false;

            xcb_get_property_cookie_t propertyCookie =
                xcb_get_property(
                    conn,
                    0,
                    window,
                    stateReply->atom,
                    XCB_ATOM_ATOM,
                    0,
                    1024);

            xcb_get_property_reply_t* propertyReply =
                xcb_get_property_reply(
                    conn,
                    propertyCookie,
                    nullptr);

            b8 found = false;

            if (propertyReply)
            {
                xcb_atom_t* atoms =
                    static_cast<xcb_atom_t*>(
                        xcb_get_property_value(propertyReply));

                i32 count =
                    xcb_get_property_value_length(propertyReply) /
                    sizeof(xcb_atom_t);

                for (i32 i = 0; i < count; ++i)
                {
                    if (atoms[i] == targetAtom)
                    {
                        found = true;
                        break;
                    }
                }

                std::free(propertyReply);
            }

            std::free(stateReply);

            return found;
        }
    }

    Window::Window(const Engine::WindowConfig& config)
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        window = xcb_generate_id(conn);

        const xcb_setup_t* setup =
            xcb_get_setup(conn);

        xcb_screen_iterator_t iterator =
            xcb_setup_roots_iterator(setup);

        xcb_screen_t* screen =
            iterator.data;

        const i32 actualWidth =
            config.width == -1
                ? screen->width_in_pixels
                : config.width;

        const i32 actualHeight =
            config.height == -1
                ? screen->height_in_pixels
                : config.height;

        // События, которые Window::HandleEvent() реально обрабатывает.
        const u32 eventMask =
            XCB_EVENT_MASK_PROPERTY_CHANGE |
            XCB_EVENT_MASK_STRUCTURE_NOTIFY |
            XCB_EVENT_MASK_FOCUS_CHANGE;

        xcb_create_window(
            conn,
            XCB_COPY_FROM_PARENT,
            window,
            screen->root,
            0,
            0,
            actualWidth,
            actualHeight,
            10,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual,
            XCB_CW_EVENT_MASK,
            &eventMask);

        // ------------------------------------------------------------
        // WM_NAME
        // ------------------------------------------------------------

        xcb_change_property(
            conn,
            XCB_PROP_MODE_REPLACE,
            window,
            XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING,
            8,
            config.title.size(),
            config.title.c_str());

        // ------------------------------------------------------------
        // _NET_WM_NAME + UTF8_STRING
        // ------------------------------------------------------------

        xcb_intern_atom_cookie_t netWmNameCookie =
            xcb_intern_atom(
                conn,
                0,
                12,
                "_NET_WM_NAME");

        xcb_intern_atom_reply_t* netWmNameReply =
            xcb_intern_atom_reply(
                conn,
                netWmNameCookie,
                nullptr);

        xcb_intern_atom_cookie_t utf8StringCookie =
            xcb_intern_atom(
                conn,
                0,
                11,
                "UTF8_STRING");

        xcb_intern_atom_reply_t* utf8StringReply =
            xcb_intern_atom_reply(
                conn,
                utf8StringCookie,
                nullptr);

        if (netWmNameReply && utf8StringReply)
        {
            netWmNameAtom =
                netWmNameReply->atom;

            utf8StringAtom =
                utf8StringReply->atom;

            xcb_change_property(
                conn,
                XCB_PROP_MODE_REPLACE,
                window,
                netWmNameAtom,
                utf8StringAtom,
                8,
                config.title.size(),
                config.title.c_str());
        }

        std::free(netWmNameReply);
        std::free(utf8StringReply);

        // ------------------------------------------------------------
        // WM_PROTOCOLS + WM_DELETE_WINDOW
        // ------------------------------------------------------------

        xcb_intern_atom_cookie_t protocolsCookie =
            xcb_intern_atom(
                conn,
                0,
                12,
                "WM_PROTOCOLS");

        xcb_intern_atom_reply_t* protocolsReply =
            xcb_intern_atom_reply(
                conn,
                protocolsCookie,
                nullptr);

        xcb_intern_atom_cookie_t deleteCookie =
            xcb_intern_atom(
                conn,
                0,
                16,
                "WM_DELETE_WINDOW");

        xcb_intern_atom_reply_t* deleteReply =
            xcb_intern_atom_reply(
                conn,
                deleteCookie,
                nullptr);

        if (protocolsReply && deleteReply)
        {
            protocolsAtom =
                protocolsReply->atom;

            deleteAtom =
                deleteReply->atom;

            xcb_change_property(
                conn,
                XCB_PROP_MODE_REPLACE,
                window,
                protocolsAtom,
                XCB_ATOM_ATOM,
                32,
                1,
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
        xcb_connection_t* conn =
            Connection::Instance().get();

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

    b8 Window::HandleEvent(
        const xcb_generic_event_t* event)
    {
        if (!event)
            return false;

        const u8 responseType =
            event->response_type & ~0x80;

        // ------------------------------------------------------------
        // Configure
        // ------------------------------------------------------------

        if (responseType == XCB_CONFIGURE_NOTIFY)
        {
            const auto* configureEvent =
                reinterpret_cast<
                    const xcb_configure_notify_event_t*
                >(event);

            if (configureEvent->width != lastKnownWidth ||
                configureEvent->height != lastKnownHeight)
            {
                resizedFlag = true;

                lastKnownWidth =
                    configureEvent->width;

                lastKnownHeight =
                    configureEvent->height;
            }

            if (configureEvent->x != lastKnownX ||
                configureEvent->y != lastKnownY)
            {
                movedFlag = true;

                lastKnownX =
                    configureEvent->x;

                lastKnownY =
                    configureEvent->y;
            }

            return true;
        }

        // ------------------------------------------------------------
        // Focus
        // ------------------------------------------------------------

        if (responseType == XCB_FOCUS_IN)
        {
            hasFocus = true;
            return true;
        }

        if (responseType == XCB_FOCUS_OUT)
        {
            hasFocus = false;
            return true;
        }

        // ------------------------------------------------------------
        // Property change
        // ------------------------------------------------------------

        if (responseType == XCB_PROPERTY_NOTIFY)
        {
            xcb_connection_t* conn =
                Connection::Instance().get();

            // Fullscreen
            xcb_intern_atom_cookie_t fullscreenCookie =
                xcb_intern_atom(
                    conn,
                    0,
                    24,
                    "_NET_WM_STATE_FULLSCREEN");

            xcb_intern_atom_reply_t* fullscreenReply =
                xcb_intern_atom_reply(
                    conn,
                    fullscreenCookie,
                    nullptr);

            if (fullscreenReply)
            {
                isFullscreen =
                    NetWmStateContains(
                        conn,
                        window,
                        fullscreenReply->atom);

                std::free(fullscreenReply);
            }

            // Always on top
            xcb_intern_atom_cookie_t aboveCookie =
                xcb_intern_atom(
                    conn,
                    0,
                    20,
                    "_NET_WM_STATE_ABOVE");

            xcb_intern_atom_reply_t* aboveReply =
                xcb_intern_atom_reply(
                    conn,
                    aboveCookie,
                    nullptr);

            if (aboveReply)
            {
                isAlwaysOnTop =
                    NetWmStateContains(
                        conn,
                        window,
                        aboveReply->atom);

                std::free(aboveReply);
            }

            return true;
        }

        // ------------------------------------------------------------
        // Client message
        // ------------------------------------------------------------

        if (responseType != XCB_CLIENT_MESSAGE)
            return false;

        const auto* clientMessage =
            reinterpret_cast<
                const xcb_client_message_event_t*
            >(event);

        if (clientMessage->window != window)
            return false;

        if (clientMessage->type != protocolsAtom)
            return false;

        if (clientMessage->data.data32[0] != deleteAtom)
            return false;

        closeRequested = true;

        return true;
    }

    b8 Window::PollEvent()
    {
        xcb_generic_event_t* event =
            EventQueue::Instance().PollEvent(window);

        if (!event)
            return false;

        const b8 handled =
            HandleEvent(event);

        std::free(event);

        return handled;
    }

    b8 Window::ShouldClose() const
    {
        return closeRequested;
    }

    void Window::SetTitle(
        const Engine::String& title)
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        xcb_change_property(
            conn,
            XCB_PROP_MODE_REPLACE,
            window,
            XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING,
            8,
            title.size(),
            title.c_str());

        if (netWmNameAtom && utf8StringAtom)
        {
            xcb_change_property(
                conn,
                XCB_PROP_MODE_REPLACE,
                window,
                netWmNameAtom,
                utf8StringAtom,
                8,
                title.size(),
                title.c_str());
        }

        xcb_flush(conn);
    }

    std::pair<i32, i32> Window::GetPosition() const
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        xcb_get_geometry_cookie_t cookie =
            xcb_get_geometry(
                conn,
                window);

        xcb_get_geometry_reply_t* reply =
            xcb_get_geometry_reply(
                conn,
                cookie,
                nullptr);

        if (!reply)
            return {0, 0};

        xcb_translate_coordinates_cookie_t translateCookie =
            xcb_translate_coordinates(
                conn,
                window,
                reply->root,
                0,
                0);

        xcb_translate_coordinates_reply_t* translateReply =
            xcb_translate_coordinates_reply(
                conn,
                translateCookie,
                nullptr);

        std::pair<i32, i32> result{0, 0};

        if (translateReply)
        {
            result = {
                translateReply->dst_x,
                translateReply->dst_y
            };

            std::free(translateReply);
        }

        std::free(reply);

        return result;
    }

    void Window::SetPosition(
        i32 x,
        i32 y)
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        const u32 values[] = {
            static_cast<u32>(x),
            static_cast<u32>(y)
        };

        xcb_configure_window(
            conn,
            window,
            XCB_CONFIG_WINDOW_X |
            XCB_CONFIG_WINDOW_Y,
            values);

        xcb_flush(conn);
    }

    std::pair<i32, i32> Window::GetSize() const
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        xcb_get_geometry_cookie_t cookie =
            xcb_get_geometry(
                conn,
                window);

        xcb_get_geometry_reply_t* reply =
            xcb_get_geometry_reply(
                conn,
                cookie,
                nullptr);

        if (!reply)
            return {0, 0};

        const std::pair<i32, i32> result{
            reply->width,
            reply->height
        };

        std::free(reply);

        return result;
    }

    void Window::SetSize(
        i32 width,
        i32 height)
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        const u32 values[] = {
            static_cast<u32>(width),
            static_cast<u32>(height)
        };

        xcb_configure_window(
            conn,
            window,
            XCB_CONFIG_WINDOW_WIDTH |
            XCB_CONFIG_WINDOW_HEIGHT,
            values);

        xcb_flush(conn);
    }

    void Window::SetResizable(
        b8 resizable)
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        isResizable = resizable;

        struct SizeHints
        {
            u32 flags;

            i32 x;
            i32 y;

            i32 width;
            i32 height;

            i32 minWidth;
            i32 minHeight;

            i32 maxWidth;
            i32 maxHeight;

            i32 widthIncrement;
            i32 heightIncrement;

            i32 minAspectNumerator;
            i32 minAspectDenominator;

            i32 maxAspectNumerator;
            i32 maxAspectDenominator;

            i32 baseWidth;
            i32 baseHeight;

            u32 winGravity;
        } hints = {};

        constexpr u32 PMinSize = 1 << 4;
        constexpr u32 PMaxSize = 1 << 5;

        const auto [width, height] =
            GetSize();

        hints.flags =
            PMinSize |
            PMaxSize;

        if (!resizable)
        {
            hints.minWidth = width;
            hints.minHeight = height;

            hints.maxWidth = width;
            hints.maxHeight = height;
        }
        else
        {
            hints.minWidth = 1;
            hints.minHeight = 1;

            hints.maxWidth = 32767;
            hints.maxHeight = 32767;
        }

        xcb_change_property(
            conn,
            XCB_PROP_MODE_REPLACE,
            window,
            XCB_ATOM_WM_NORMAL_HINTS,
            XCB_ATOM_WM_SIZE_HINTS,
            32,
            sizeof(hints) / sizeof(u32),
            &hints);

        xcb_flush(conn);
    }

    b8 Window::IsResizable() const
    {
        return isResizable;
    }

    void Window::Show()
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        xcb_map_window(
            conn,
            window);

        xcb_flush(conn);
    }

    void Window::Hide()
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        xcb_unmap_window(
            conn,
            window);

        xcb_flush(conn);
    }

    void Window::Minimize()
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        const xcb_setup_t* setup =
            xcb_get_setup(conn);

        xcb_screen_t* screen =
            xcb_setup_roots_iterator(setup).data;

        xcb_intern_atom_cookie_t cookie =
            xcb_intern_atom(
                conn,
                0,
                16,
                "WM_CHANGE_STATE");

        xcb_intern_atom_reply_t* reply =
            xcb_intern_atom_reply(
                conn,
                cookie,
                nullptr);

        if (!reply)
            return;

        xcb_client_message_event_t event = {};

        event.response_type =
            XCB_CLIENT_MESSAGE;

        event.format = 32;
        event.window = window;
        event.type = reply->atom;

        // IconicState
        event.data.data32[0] = 3;

        xcb_send_event(
            conn,
            0,
            screen->root,
            XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
            XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
            reinterpret_cast<const char*>(&event));

        std::free(reply);

        xcb_flush(conn);
    }

    void Window::Maximize()
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        const xcb_setup_t* setup =
            xcb_get_setup(conn);

        xcb_screen_t* screen =
            xcb_setup_roots_iterator(setup).data;

        xcb_intern_atom_cookie_t verticalCookie =
            xcb_intern_atom(
                conn,
                0,
                24,
                "_NET_WM_STATE_MAXIMIZED_VERT");

        xcb_intern_atom_reply_t* verticalReply =
            xcb_intern_atom_reply(
                conn,
                verticalCookie,
                nullptr);

        xcb_intern_atom_cookie_t horizontalCookie =
            xcb_intern_atom(
                conn,
                0,
                24,
                "_NET_WM_STATE_MAXIMIZED_HORZ");

        xcb_intern_atom_reply_t* horizontalReply =
            xcb_intern_atom_reply(
                conn,
                horizontalCookie,
                nullptr);

        if (verticalReply && horizontalReply)
        {
            SendNetWmStateMessage(
                conn,
                window,
                screen,
                1,
                verticalReply->atom,
                horizontalReply->atom);
        }

        std::free(verticalReply);
        std::free(horizontalReply);
    }

    void Window::Restore()
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        const xcb_setup_t* setup =
            xcb_get_setup(conn);

        xcb_screen_t* screen =
            xcb_setup_roots_iterator(setup).data;

        xcb_intern_atom_cookie_t verticalCookie =
            xcb_intern_atom(
                conn,
                0,
                24,
                "_NET_WM_STATE_MAXIMIZED_VERT");

        xcb_intern_atom_reply_t* verticalReply =
            xcb_intern_atom_reply(
                conn,
                verticalCookie,
                nullptr);

        xcb_intern_atom_cookie_t horizontalCookie =
            xcb_intern_atom(
                conn,
                0,
                24,
                "_NET_WM_STATE_MAXIMIZED_HORZ");

        xcb_intern_atom_reply_t* horizontalReply =
            xcb_intern_atom_reply(
                conn,
                horizontalCookie,
                nullptr);

        if (verticalReply && horizontalReply)
        {
            SendNetWmStateMessage(
                conn,
                window,
                screen,
                0,
                verticalReply->atom,
                horizontalReply->atom);
        }

        std::free(verticalReply);
        std::free(horizontalReply);
    }

    void Window::SetFullscreen(
        b8 fullscreen)
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        const xcb_setup_t* setup =
            xcb_get_setup(conn);

        xcb_screen_t* screen =
            xcb_setup_roots_iterator(setup).data;

        xcb_intern_atom_cookie_t cookie =
            xcb_intern_atom(
                conn,
                0,
                24,
                "_NET_WM_STATE_FULLSCREEN");

        xcb_intern_atom_reply_t* reply =
            xcb_intern_atom_reply(
                conn,
                cookie,
                nullptr);

        if (reply)
        {
            SendNetWmStateMessage(
                conn,
                window,
                screen,
                fullscreen ? 1 : 0,
                reply->atom);

            std::free(reply);
        }

        xcb_flush(conn);
    }

    b8 Window::IsFullscreen() const
    {
        return isFullscreen;
    }

    void Window::SetAlwaysOnTop(
        b8 onTop)
    {
        xcb_connection_t* conn =
            Connection::Instance().get();

        const xcb_setup_t* setup =
            xcb_get_setup(conn);

        xcb_screen_t* screen =
            xcb_setup_roots_iterator(setup).data;

        xcb_intern_atom_cookie_t cookie =
            xcb_intern_atom(
                conn,
                0,
                20,
                "_NET_WM_STATE_ABOVE");

        xcb_intern_atom_reply_t* reply =
            xcb_intern_atom_reply(
                conn,
                cookie,
                nullptr);

        if (reply)
        {
            SendNetWmStateMessage(
                conn,
                window,
                screen,
                onTop ? 1 : 0,
                reply->atom);

            std::free(reply);
        }

        xcb_flush(conn);
    }

    b8 Window::IsAlwaysOnTop() const
    {
        return isAlwaysOnTop;
    }

    b8 Window::WasResized()
    {
        const b8 result =
            resizedFlag;

        resizedFlag = false;

        return result;
    }

    b8 Window::WasMoved()
    {
        const b8 result =
            movedFlag;

        movedFlag = false;

        return result;
    }

    b8 Window::HasFocus() const
    {
        return hasFocus;
    }
}