{
    files = {
        "Empire.Render/X11/OpenGL/Context.cpp"
    },
    depfiles = "Context.o: Empire.Render/X11/OpenGL/Context.cpp  Empire.Render/X11/OpenGL/Context.hpp ../Aliases.hpp  Empire.App/Xcb/Connection.hpp\
",
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
            "-IEmpire.Render",
            "-I..",
            "-DNDEBUG"
        }
    }
}