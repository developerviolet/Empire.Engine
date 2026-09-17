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
    depfiles = "main.o: Tests/main.cpp Empire.Window/Window.hpp  Empire.Window/WindowConfig.hpp ../Aliases.hpp\
",
    files = {
        "Tests/main.cpp"
    }
}