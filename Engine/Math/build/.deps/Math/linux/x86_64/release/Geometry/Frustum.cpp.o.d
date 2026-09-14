{
    depfiles_format = "gcc",
    files = {
        "Geometry/Frustum.cpp"
    },
    depfiles = "Frustum.o: Geometry/Frustum.cpp Geometry/Frustum.hpp ../Aliases.hpp  Geometry/Plane.hpp Vector/Vector3.hpp Matrix/Matrix4.hpp  Vector/Vector4.hpp\
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