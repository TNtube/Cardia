set_project("Utopia")
set_version("0.0.0")

add_rules("mode.debug", "mode.release")
set_languages("cxx17")

add_requires("spdlog v1.9.0") -- latest version at the time


outputdir = "$(mode)-$(os)-$(arch)"


target("Utopia")
    set_kind("shared")
    set_basename("Utopia")

    set_targetdir("/build/" .. outputdir .. "/Utopia/bin")
    set_objectdir("/build/" .. outputdir .. "/Utopia/obj")
    

    add_files("Utopia/src/**.cpp")
    add_headerfiles("Utopia/src/**.h")
    add_includedirs("Utopia/src/", {public = true})

    add_packages("spdlog")

    if is_plat("windows") then
        add_defines("UT_PLATFORM_WINDOWS", "UT_BUILD_DLL")
    end

target("Sandbox")
    set_kind("binary")
    set_basename("Sandbox")

    set_targetdir("/build/" .. outputdir .. "/SandBox/bin")
    set_objectdir("/build/" .. outputdir .. "/SandBox/obj")

    add_files("Sandbox/src/**.cpp")
    add_headerfiles("Sandbox/src/**.h")

    add_packages("spdlog")
    add_deps("Utopia")

    if is_plat("windows") then
        add_defines("UT_PLATFORM_WINDOWS")
    end
