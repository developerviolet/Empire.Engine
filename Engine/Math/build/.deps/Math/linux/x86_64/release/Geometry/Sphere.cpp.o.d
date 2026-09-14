{
    depfiles_format = "gcc",
    files = {
        "Geometry/Sphere.cpp"
    },
    depfiles = "Sphere.o: Geometry/Sphere.cpp Geometry/Sphere.hpp ../Aliases.hpp  Vector/Vector3.hpp\
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