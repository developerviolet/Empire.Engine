{
    depfiles_format = "gcc",
    files = {
        "Vector/Vector4.cpp"
    },
    depfiles = "Vector4.o: Vector/Vector4.cpp Vector/Vector4.hpp ../Aliases.hpp\
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