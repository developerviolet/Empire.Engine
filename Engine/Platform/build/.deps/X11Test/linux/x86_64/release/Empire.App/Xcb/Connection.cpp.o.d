{
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
    depfiles = "Connection.o: Empire.App/Xcb/Connection.cpp Empire.App/Xcb/Connection.hpp\
",
    files = {
        "Empire.App/Xcb/Connection.cpp"
    }
}