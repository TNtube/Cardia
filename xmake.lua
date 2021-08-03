set_project("Utopia GameEngine")
set_version("0.0.0")

add_rules("mode.debug", "mode.release")
set_languages("cxx17")

add_requires("spdlog v1.9.0") -- latest version at the time

target("utopia")
    set_kind("shared")
    set_basename("Utopia")

    add_files("Utopia/src/Utopia/*.cpp")
    add_headerfiles("Utopia/src/Utopia/*.h")
    add_includedirs("Utopia/src/", {public = true})

    add_packages("spdlog")

    if is_plat("windows") then
        add_defines("UT_PLATFORM_WINDOWS", "UT_BUILD_DLL")
    end

target("sandbox")
    set_kind("binary")
    set_basename("Sandbox")

    add_files("Sandbox/src/*.cpp")
    add_headerfiles("Sandbox/src/*.h")

    add_packages("spdlog")
    add_deps("utopia")

    if is_plat("windows") then
        add_defines("UT_PLATFORM_WINDOWS")
    end
