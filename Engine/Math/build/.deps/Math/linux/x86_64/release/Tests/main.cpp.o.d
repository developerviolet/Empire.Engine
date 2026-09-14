{
    depfiles_format = "gcc",
    files = {
        "Tests/main.cpp"
    },
    depfiles = "main.o: Tests/main.cpp Vector/Vector3.hpp ../Aliases.hpp  Matrix/Matrix4.hpp Vector/Vector4.hpp Quaternion/Quaternion.hpp  Transform/Transform.hpp Math.hpp\
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