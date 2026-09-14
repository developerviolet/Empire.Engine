{
    depfiles_format = "gcc",
    files = {
        "Math.cpp"
    },
    depfiles = "Math.o: Math.cpp Math.hpp ../Aliases.hpp\
",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-I.",
            "-I..",
            "-IVector",
            "-IMatrix",
            "-IQuaternion",
            "-ITransform"
        }
    }
}