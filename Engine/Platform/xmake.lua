set_project("Empire.Platform")

set_languages("cxx26")

add_rules("mode.debug", "mode.release")

target("X11Test")
    set_kind("binary")

    add_files(
        "Empire.Window/*.cpp",
        "Empire.Window/Xcb/*.cpp",
        "Empire.App/Xcb/*.cpp",
        "Empire.Render/X11/OpenGL/*.cpp",
        "Tests/*.cpp"
    )

    add_includedirs(
        ".",
        "Empire.App",
        "Empire.Window",
        "Empire.Render",
        ".."
    )

    add_links(
        "xcb",
        "X11",
        "GL",
        "GLX",
        "glbinding"
    )