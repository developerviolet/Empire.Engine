{
    files = {
        "Empire.App/Xcb/Connection.cpp"
    },
    depfiles_format = "gcc",
    depfiles = "Connection.o: Empire.App/Xcb/Connection.cpp Empire.App/Xcb/Connection.hpp\
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