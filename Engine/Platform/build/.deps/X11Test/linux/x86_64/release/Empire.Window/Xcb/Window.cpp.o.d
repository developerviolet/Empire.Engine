{
    files = {
        "Empire.Window/Xcb/Window.cpp"
    },
    depfiles_format = "gcc",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++26",
            "-I.",
            "-IEmpire.App",
            "-IEmpire.Window",
            "-I..",
            "-DNDEBUG"
        }
    },
    depfiles = "Window.o: Empire.Window/Xcb/Window.cpp Empire.Window/Xcb/Window.hpp  Empire.Window/WindowConfig.hpp ../Aliases.hpp  Empire.App/Xcb/Connection.hpp Empire.App/Xcb/EventQueue.hpp\
"
}