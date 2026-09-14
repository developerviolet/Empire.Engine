{
    depfiles_format = "gcc",
    files = {
        "Transform/Transform.cpp"
    },
    depfiles = "Transform.o: Transform/Transform.cpp Transform/Transform.hpp  ../Aliases.hpp Vector/Vector3.hpp Quaternion/Quaternion.hpp  Matrix/Matrix4.hpp Vector/Vector4.hpp\
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