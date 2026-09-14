{
    depfiles_format = "gcc",
    files = {
        "Geometry/CatmullRom.cpp"
    },
    depfiles = "CatmullRom.o: Geometry/CatmullRom.cpp Geometry/CatmullRom.hpp  ../Aliases.hpp Vector/Vector3.hpp\
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