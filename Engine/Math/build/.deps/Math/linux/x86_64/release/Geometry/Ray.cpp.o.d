{
    depfiles_format = "gcc",
    files = {
        "Geometry/Ray.cpp"
    },
    depfiles = "Ray.o: Geometry/Ray.cpp Geometry/Ray.hpp ../Aliases.hpp  Vector/Vector3.hpp\
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