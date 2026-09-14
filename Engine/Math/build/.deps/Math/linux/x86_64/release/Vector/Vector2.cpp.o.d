{
    depfiles_format = "gcc",
    files = {
        "Vector/Vector2.cpp"
    },
    depfiles = "Vector2.o: Vector/Vector2.cpp Vector/Vector2.hpp ../Aliases.hpp\
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