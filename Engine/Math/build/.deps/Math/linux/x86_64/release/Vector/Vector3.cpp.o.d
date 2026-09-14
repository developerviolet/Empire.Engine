{
    depfiles_format = "gcc",
    files = {
        "Vector/Vector3.cpp"
    },
    depfiles = "Vector3.o: Vector/Vector3.cpp Vector/Vector3.hpp ../Aliases.hpp\
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