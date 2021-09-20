set_xmakever("2.5.6")

set_project("CardiaGameEngine")
set_version("0.0.0")

set_allowedplats("windows", "linux", "macosx")
set_allowedarchs("windows|x64", "linux|x86_64", "macosx|x86_64")

add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
set_languages("cxx17")

add_requires("spdlog v1.9.0")                                               -- latest version at the time
add_requires("glfw 3.3.4")                                                  -- latest version at the time
add_requires("glad v0.1.34")                                                -- latest version at the time
add_requires("imgui v1.83-docking", {configs = {glfw_opengl3 = true}})      -- latest version at the time
add_requires("glm 0.9.9+8")                                                 -- latest version at the time

local outputdir = "$(mode)-$(os)-$(arch)"

target("Cardia")
    set_kind("static")

    set_targetdir("build/" .. outputdir .. "/Cardia/bin")
    set_objectdir("build/" .. outputdir .. "/Cardia/obj")

    set_pcxxheader("Cardia/include/cdpch.hpp")

    add_files("Cardia/src/**.cpp")
    add_headerfiles("Cardia/include/**.hpp")
    add_includedirs("Cardia/include/", {public = true})

    add_packages("spdlog")
    add_packages("glfw")
    add_packages("glad")
    add_packages("imgui")
    add_packages("glm")

    if is_plat("windows") then
        add_defines("CD_PLATFORM_WINDOWS", "CD_BUILD_DLL")
    end

    if is_mode("debug") then
        add_defines("CD_DEBUG")
    end

target("SandBox")
    set_kind("binary")

    set_targetdir("build/" .. outputdir .. "/SandBox/bin")
    set_objectdir("build/" .. outputdir .. "/SandBox/obj")

    add_files("SandBox/src/**.cpp")
    add_headerfiles("SandBox/src/**.hpp")

    add_packages("spdlog")
    add_packages("imgui")
    add_packages("glm")
    add_deps("Cardia")

    if is_plat("windows") then
        add_defines("CD_PLATFORM_WINDOWS")
    end
    if is_mode("debug") then
        add_defines("CD_DEBUG")
    end
