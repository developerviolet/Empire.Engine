{
    files = {
        "build/.objs/X11Test/linux/x86_64/release/Empire.App/Xcb/Connection.cpp.o",
        "build/.objs/X11Test/linux/x86_64/release/Empire.App/Xcb/EventQueue.cpp.o",
        "build/.objs/X11Test/linux/x86_64/release/Empire.Render/X11/OpenGL/Context.cpp.o",
        "build/.objs/X11Test/linux/x86_64/release/Empire.Window/Window.cpp.o",
        "build/.objs/X11Test/linux/x86_64/release/Empire.Window/Xcb/Window.cpp.o",
        "build/.objs/X11Test/linux/x86_64/release/Tests/main.cpp.o"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-s",
            "-lxcb",
            "-lX11",
            "-lGL",
            "-lGLX",
            "-lglbinding"
        }
    }
}