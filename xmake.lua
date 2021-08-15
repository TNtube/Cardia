set_xmakever("2.5.6")

add_repositories("test-repo xmake-repo")

set_project("UtopiaGameEngine")
set_version("0.0.0")

set_allowedplats("windows", "linux", "macosx")
set_allowedarchs("windows|x64", "linux|x86_64", "macosx|x86_64")

add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
set_languages("cxx17")

add_requires("spdlog v1.9.0") -- latest version at the time
add_requires("glfw 3.3.4")    -- latest version at the time
add_requires("glad v0.1.34")  -- latest version at the time
add_requires("imgui v1.83-docking")

local outputdir = "$(mode)-$(os)-$(arch)"

target("Utopia")
    set_kind("shared")

    set_targetdir("/build/" .. outputdir .. "/Utopia/bin")
    set_objectdir("/build/" .. outputdir .. "/Utopia/obj")

    set_pcxxheader("Utopia/src/utpch.hpp")

    add_files("Utopia/src/**.cpp")
    add_headerfiles("Utopia/src/**.hpp")
    add_includedirs("Utopia/src/", {public = true})

    add_packages("spdlog")
    add_packages("glfw")
    add_packages("glad")
    add_packages("imgui")

    if is_plat("windows") then
        add_defines("UT_PLATFORM_WINDOWS", "UT_BUILD_DLL")
    end

    if is_mode("debug") then
        add_defines("UT_DEBUG")
    end

target("SandBox")
    set_kind("binary")

    set_targetdir("/build/" .. outputdir .. "/SandBox/bin")
    set_objectdir("/build/" .. outputdir .. "/SandBox/obj")

    add_files("SandBox/src/**.cpp")
    add_headerfiles("SandBox/src/**.hpp")

    add_packages("spdlog")
    add_deps("Utopia")

    if is_plat("windows") then
        add_defines("UT_PLATFORM_WINDOWS")
    end
    if is_mode("debug") then
        add_defines("UT_DEBUG")
    end

    after_build(function (target)
        os.cp("/build/" .. outputdir .. "/Utopia/bin/Utopia.dll", "/build/" .. outputdir .. "/SandBox/bin")
    end)
