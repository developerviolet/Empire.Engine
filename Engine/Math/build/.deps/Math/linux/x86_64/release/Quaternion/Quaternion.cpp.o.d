{
    depfiles_format = "gcc",
    files = {
        "Quaternion/Quaternion.cpp"
    },
    depfiles = "Quaternion.o: Quaternion/Quaternion.cpp Quaternion/Quaternion.hpp  ../Aliases.hpp Vector/Vector3.hpp Matrix/Matrix4.hpp Vector/Vector4.hpp\
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