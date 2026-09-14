{
    depfiles_format = "gcc",
    files = {
        "Geometry/Bezier.cpp"
    },
    depfiles = "Bezier.o: Geometry/Bezier.cpp Geometry/Bezier.hpp ../Aliases.hpp  Vector/Vector3.hpp\
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