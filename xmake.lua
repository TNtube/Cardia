set_xmakever("2.5.9")

set_project("CardiaGameEngine")
set_version("0.0.0")

set_allowedplats("windows", "linux", "macosx")
set_allowedarchs("windows|x64", "linux|x64", "macosx|x86_64")

add_rules("mode.debug", "mode.release", "mode.asan")
add_rules("plugin.vsxmake.autoupdate")
set_languages("cxx2a")

add_repositories("my-repo https://github.com/TNtube/xmake-repo.git")

add_requires("spdlog v1.9.0")                                               -- latest version at the time
add_requires("glfw 3.3.6")                                                  -- latest version at the time
add_requires("glad v0.1.36")                                                -- latest version at the time
add_requires("imgui v1.83-docking", {configs = {glfw_opengl3 = true}})      -- latest version at the time
add_requires("glm 0.9.9+8")                                                 -- latest version at the time
add_requires("entt v3.10.0")                                                -- latest version at the time
add_requires("imguizmo 1.83")                                               -- latest version at the time
add_requireconfs("imguizmo.imgui", {override = true, version = "v1.83-docking", configs = {glfw_opengl3 = true}}) -- config sub imgui module

local outputdir = "$(mode)-$(os)-$(arch)"

rule("cp-assets")
    after_build(function (target)
            os.cp(target:name() .. "/assets", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
        end)

target("Cardia")
    set_kind("static")

    set_targetdir("build/" .. outputdir .. "/Cardia/bin")
    set_objectdir("build/" .. outputdir .. "/Cardia/obj")

    set_pcxxheader("Cardia/include/cdpch.hpp")

    add_files("Cardia/src/**.cpp")
    add_headerfiles("Cardia/include/Cardia/**.hpp")
    add_headerfiles("Cardia/include/Cardia.hpp")
    add_includedirs("Cardia/include/", {public = true})
    add_includedirs("Cardia/vendor/", {public = true})

    add_packages("spdlog")
    add_packages("glfw")
    add_packages("glad")
    add_packages("imgui")
    add_packages("glm")
    add_packages("entt")

    if is_mode("debug") then
        add_defines("CD_DEBUG")
    end

target("SandBox")
    set_kind("binary")
    add_rules("cp-assets")

    set_targetdir("build/" .. outputdir .. "/SandBox/bin")
    set_objectdir("build/" .. outputdir .. "/SandBox/obj")

    add_headerfiles("SandBox/include/**.hpp")
    add_files("SandBox/src/**.cpp")
    add_includedirs("SandBox/include/", {public = true})

    add_packages("spdlog")
    add_packages("imgui")
    add_packages("glm")
    add_packages("entt")
    add_deps("Cardia")

    if is_mode("debug") then
        add_defines("CD_DEBUG")
    end

target("CardiaTor")
    set_kind("binary")
    add_rules("cp-assets")

    set_targetdir("build/" .. outputdir .. "/CardiaTor/bin")
    set_objectdir("build/" .. outputdir .. "/CardiaTor/obj")

    add_headerfiles("CardiaTor/include/**.hpp")
    add_files("CardiaTor/src/**.cpp")
    add_includedirs("CardiaTor/include/", {public = true})

    add_packages("spdlog")
    add_packages("imgui")
    add_packages("glm")
    add_packages("entt")
    add_packages("imguizmo")
    add_deps("Cardia")

    if is_mode("debug") then
        add_defines("CD_DEBUG")
    end
