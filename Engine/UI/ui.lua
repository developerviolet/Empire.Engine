set_project("Empire.Platform")

set_languages("cxx26")

add_rules("mode.debug", "mode.release")

target("X11Test")
    set_kind("binary")

    add_files(
        "Empire.Window/Xcb/*.cpp",
        "Empire.App/Xcb/*.cpp",
        "Tests/*.cpp"
    )

    add_includedirs(
        "Empire.App/Xcb",
        "Empire.Window/Xcb"
    )

    add_links("xcb")
