{
    depfiles_format = "gcc",
    files = {
        "Matrix/Matrix2.cpp"
    },
    depfiles = "Matrix2.o: Matrix/Matrix2.cpp Matrix/Matrix2.hpp ../Aliases.hpp  Vector/Vector2.hpp Math.hpp\
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