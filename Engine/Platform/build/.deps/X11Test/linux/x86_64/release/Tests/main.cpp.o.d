{
    files = {
        "Tests/main.cpp"
    },
    depfiles_format = "gcc",
    depfiles = "main.o: Tests/main.cpp Empire.Window/Window.hpp  Empire.Window/WindowConfig.hpp ../Aliases.hpp\
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