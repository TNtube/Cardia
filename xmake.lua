set_xmakever("2.5.9")

set_project("CardiaGameEngine")
set_version("0.0.0")

set_allowedplats("windows", "linux", "macosx")
set_allowedarchs("windows|x64", "linux|x64", "macosx|x86_64")

add_rules("mode.debug", "mode.release", "mode.asan")
add_rules("plugin.vsxmake.autoupdate")
set_languages("cxx20")
set_optimize("fastest")

add_requires("spdlog v1.9.0")
add_requires("glfw 3.3.8")
add_requires("glad v0.1.36")
add_requires("imgui v1.83-docking", {configs = {glfw_opengl3 = true}})
add_requires("glm 0.9.9+8")
add_requires("entt v3.10.0")
add_requires("nativefiledialog 1.1.6")
add_requires("jsoncpp 1.9.5")

add_requires("imguizmo 1.83")
add_requireconfs("imguizmo.imgui", {override = true, version = "v1.83-docking", configs = {glfw_opengl3 = true}}) -- config sub imgui module

add_requires("python 3.9.13")
add_requires("pybind11")
add_requireconfs("pybind11.python", {override = true, config = {kind = "binary"}, version = "3.9.13"})

local outputdir = "$(mode)-$(os)-$(arch)"

rule("cp-resources")
    after_build(function (target)
        os.cp(target:name() .. "/resources", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
    end)

rule("python-env")
    on_load(function (target)
		target:add("packages", "python")

		local pythonDir = target:pkg("python"):installdir()
        print("Python dir is : " .. pythonDir)
        
        os.setenv("PYTHONPATH", path.join(pythonDir, "Lib", "site-packages"))
        os.setenv("PYTHONHOME", pythonDir)
	end)

target("Cardia")
    set_kind("static")
    set_runtimes("MT")

    set_targetdir("build/" .. outputdir .. "/Cardia/bin")
    set_objectdir("build/" .. outputdir .. "/Cardia/obj")

    set_pcxxheader("Cardia/include/cdpch.hpp")

    add_files("Cardia/**.cpp")
    add_headerfiles("Cardia/**.hpp", "Cardia/**.h")
    add_includedirs("Cardia/include/", {public = true})
    add_includedirs("Cardia/vendor/", {public = true})

    add_packages("spdlog", { public = true })
    add_packages("glfw")
    add_packages("glad")
    add_packages("imgui", { public = true })
    add_packages("glm", { public = true })
    add_packages("entt", { public = true })
    add_packages("jsoncpp", { public = true })
    add_packages("python", { public = true })
    add_packages("pybind11", { public = true })
    
    add_rules("python-env")

    if is_mode("debug") then
        add_defines("CD_DEBUG")
    end

target("SandBox")
    set_kind("binary")
    set_runtimes("MT")
    add_rules("cp-resources")

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
    set_runtimes("MT")
    add_rules("cp-resources")

    set_targetdir("build/" .. outputdir .. "/CardiaTor/bin")
    set_objectdir("build/" .. outputdir .. "/CardiaTor/obj")

    add_headerfiles("CardiaTor/include/**.hpp")
    add_headerfiles("CardiaTor/resources/**") -- a hack for the moment
    add_files("CardiaTor/src/**.cpp", "CardiaTor/resources/logo/resource.rc")
    add_includedirs("CardiaTor/include/", {public = true})

    add_packages("imguizmo")
    add_packages("nativefiledialog")
    add_deps("Cardia")

    if is_mode("debug") then
        add_defines("CD_DEBUG")
    end
