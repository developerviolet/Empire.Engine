set_project("Empire.Platform")

set_languages("cxx26")

add_rules("mode.debug", "mode.release")

target("X11Test")
    set_kind("binary")

    add_files(
        "Empire.Window/X11/*.cpp",
        "Empire.App/X11/*.cpp",
        "Tests/*.cpp"
    )

    add_includedirs(
        "Empire.App/X11",
        "Empire.Window/X11"
    )

    add_links("xcb")
