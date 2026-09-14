{
    depfiles_format = "gcc",
    files = {
        "Geometry/Plane.cpp"
    },
    depfiles = "Plane.o: Geometry/Plane.cpp Geometry/Plane.hpp ../Aliases.hpp  Vector/Vector3.hpp\
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