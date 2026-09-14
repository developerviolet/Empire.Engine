{
    depfiles_format = "gcc",
    files = {
        "Matrix/Matrix3.cpp"
    },
    depfiles = "Matrix3.o: Matrix/Matrix3.cpp Matrix/Matrix3.hpp ../Aliases.hpp  Vector/Vector3.hpp Math.hpp\
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