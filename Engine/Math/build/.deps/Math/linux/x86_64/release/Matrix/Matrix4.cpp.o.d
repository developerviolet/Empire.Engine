{
    depfiles_format = "gcc",
    files = {
        "Matrix/Matrix4.cpp"
    },
    depfiles = "Matrix4.o: Matrix/Matrix4.cpp Matrix/Matrix4.hpp ../Aliases.hpp  Vector/Vector3.hpp Vector/Vector4.hpp\
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