{
    depfiles_format = "gcc",
    files = {
        "Geometry/AABB.cpp"
    },
    depfiles = "AABB.o: Geometry/AABB.cpp Geometry/AABB.hpp ../Aliases.hpp  Vector/Vector3.hpp\
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