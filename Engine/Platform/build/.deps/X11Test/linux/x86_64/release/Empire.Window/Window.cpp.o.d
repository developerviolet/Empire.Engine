{
    depfiles_format = "gcc",
    depfiles = "Window.o: Empire.Window/Window.cpp Empire.Window/Window.hpp  Empire.Window/WindowConfig.hpp ../Aliases.hpp  Empire.Window/Xcb/Window.hpp\
",
    files = {
        "Empire.Window/Window.cpp"
    },
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
    }
}