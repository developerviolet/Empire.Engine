{
    files = {
        "Empire.App/Xcb/EventQueue.cpp"
    },
    depfiles_format = "gcc",
    depfiles = "EventQueue.o: Empire.App/Xcb/EventQueue.cpp Empire.App/Xcb/EventQueue.hpp  Empire.App/Xcb/Connection.hpp\
",
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