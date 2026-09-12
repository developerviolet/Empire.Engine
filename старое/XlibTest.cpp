#include <iostream>
#include <string>
#include <format>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

struct Window
{
    std::string title = "DEvchik10 Engine";

    int width = 800;
    int height = 600;

    bool fullscreen = false;
    bool isOpen = false;

    Display* display = nullptr;
    int screen = 0;

    ::Window handle = 0;

    GC gc = 0;

    Atom wmDeleteWindow;

    void create()
    {
        display = XOpenDisplay(nullptr);

        if (!display)
        {
            std::cout << "[X11]: Failed to connect to X Server\n";
            return;
        }

        screen = DefaultScreen(display);

        handle = XCreateSimpleWindow(
            display,
            RootWindow(display, screen),
            0,
            0,
            width,
            height,
            0,
            BlackPixel(display, screen),
            0x800080
        );

        XStoreName(display, handle, title.c_str());

        wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, handle, &wmDeleteWindow, 1);

        XSelectInput(
            display,
            handle,
            ExposureMask |
            KeyPressMask |
            ButtonPressMask |
            StructureNotifyMask
        );

        gc = XCreateGC(display, handle, 0, nullptr);

        XMapWindow(display, handle);

        XFlush(display);

        fillColor(128, 0, 128);

        isOpen = true;

        std::cout << std::format(
            "[Xlib Window]: '{}' [{}x{}]\n",
            title,
            width,
            height
        );
    }

    void fillColor(uint8_t r, uint8_t g, uint8_t b)
    {
        unsigned long color =
            (r << 16) |
            (g << 8) |
            b;

        XSetForeground(display, gc, color);

        XFillRectangle(
            display,
            handle,
            gc,
            0,
            0,
            width,
            height
        );

        XFlush(display);
    }

    void waitForEvents()
    {
        XEvent event;

        XNextEvent(display, &event);

        switch (event.type)
        {
            case Expose:
            {
                fillColor(128, 0, 128);
                break;
            }

            case ClientMessage:
            {
                if ((Atom)event.xclient.data.l[0] == wmDeleteWindow)
                {
                    isOpen = false;
                }

                break;
            }
        }
    }

    void destroy()
    {
        if (!display)
            return;

        XFreeGC(display, gc);
        XDestroyWindow(display, handle);
        XCloseDisplay(display);

        display = nullptr;
    }
};

Window window;

void Init()
{
    std::cout << "[C++ Engine]: Initializing..\n";

    window.create();

    std::cout << "[C++ Engine]: Done!\n\n";
}

int main()
{
    Init();

    while (window.isOpen)
    {
        window.waitForEvents();
    }

    window.destroy();

    std::cout << "[C++ Engine]: Shutdown.\n";

    return 0;
}
