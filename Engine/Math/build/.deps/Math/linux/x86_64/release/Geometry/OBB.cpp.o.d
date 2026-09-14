{
    depfiles_format = "gcc",
    files = {
        "Geometry/OBB.cpp"
    },
    depfiles = "OBB.o: Geometry/OBB.cpp Geometry/OBB.hpp ../Aliases.hpp  Vector/Vector3.hpp Quaternion/Quaternion.hpp Matrix/Matrix4.hpp  Vector/Vector4.hpp\
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