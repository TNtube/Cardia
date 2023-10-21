set_xmakever("2.5.9")

set_project("CardiaGameEngine")
set_version("0.0.0")

set_allowedplats("windows", "linux", "macosx")
set_allowedarchs("windows|x64", "linux|x86_64", "macosx|x86_64")

add_rules("mode.debug", "mode.release", "mode.asan")
add_rules("plugin.vsxmake.autoupdate")
set_languages("c++20")
set_optimize("fastest")

add_requires(
        "spdlog v1.9.0", "glfw 3.3.8", "entt v3.12.2",
        "nativefiledialog 1.1.6", "jsoncpp 1.9.5", "assimp v5.2.5",
        "volk", "catch2 v3.4.0", "python 3.11.3", "pybind11",
        "efsw 1.3.1")

add_requires("imguizmo 1.89+WIP", {configs = { debug = is_mode("debug")}})
add_requires("imgui v1.89-docking", {configs = {glfw= true, debug = is_mode("debug")}})
add_requireconfs("imguizmo.imgui", {override = true, version = "v1.89-docking", configs = {glfw= true, debug = is_mode("debug")}}) -- config sub imgui module

add_requires("python 3.11.3")
add_requires("pybind11")
add_requireconfs("pybind11.python", {override = true, config = {kind = "binary"}, version = "3.11.3"})

local outputdir = "$(mode)-$(os)-$(arch)"

rule("cp-resources")
    after_build(function (target)
        os.cp(target:name() .. "/resources", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
    end)

target("Cardia")
    set_kind("static")
    set_runtimes("MT")

    set_targetdir("build/" .. outputdir .. "/Cardia/bin")
    set_objectdir("build/" .. outputdir .. "/Cardia/obj")

    set_pcxxheader("Cardia/include/cdpch.hpp")

    add_files("Cardia/**.cpp")
    add_headerfiles("Cardia/**.hpp", "Cardia/**.h", "Cardia/**.inl")
    add_includedirs("Cardia/include/", {public = true})
    add_includedirs("Cardia/vendor/", {public = true})

    add_packages("spdlog", "volk", "imgui", "entt", "jsoncpp", "python", "pybind11", "efsw", { public = true })
    add_packages("glfw", "assimp")

    add_defines("VK_NO_PROTOTYPES", { public = true })


--[[    after_build(function(target)
        os.execv("python", {"-m", "pip", "install", target:scriptdir().."/cardia.py/dist/cardia.py-0.0.1-py3-none-any.whl", "--force-reinstall"})
    end)

    on_load(function(target)
        local pythonDir = target:pkg("python"):installdir()

        os.setenv("PYTHONPATH", path.join(pythonDir, "Lib", "site-packages"))
        os.setenv("PYTHONHOME", pythonDir)
    end)--]]


target("SandBox")
    set_kind("binary")
    set_runtimes("MT")
    add_rules("cp-resources")

    set_targetdir("build/" .. outputdir .. "/SandBox/bin")
    set_objectdir("build/" .. outputdir .. "/SandBox/obj")

    add_headerfiles("SandBox/include/**.hpp")
    add_files("SandBox/src/**.cpp")
    add_includedirs("SandBox/include/", {public = true})

    add_deps("Cardia")


target("CardiaTor")
    set_kind("binary")
    set_runtimes("MT")
    add_rules("cp-resources")

    set_targetdir("build/" .. outputdir .. "/CardiaTor/bin")
    set_objectdir("build/" .. outputdir .. "/CardiaTor/obj")

    add_headerfiles("CardiaTor/include/**.hpp")
    add_headerfiles("CardiaTor/resources/**") -- a hack for the moment
    add_files("CardiaTor/src/**.cpp")
    add_includedirs("CardiaTor/include/", {public = true})
    set_rundir("CardiaTor/")

    add_packages("imguizmo", "nativefiledialog")
    add_deps("Cardia")


target("Tests")
    set_kind("binary")
    set_runtimes("MT")

    set_targetdir("build/" .. outputdir .. "/Tests/bin")
    set_objectdir("build/" .. outputdir .. "/Tests/obj")

    add_headerfiles("Tests/**.hpp")
    add_files("Tests/**.cpp")
    add_includedirs("Tests/Cardia/include/", {public = true})

    add_packages("catch2")
    add_deps("Cardia")
