---@diagnostic disable: undefined-global

add_rules("mode.debug", "mode.release")
set_languages("c++20")

includes("EngineSquared/xmake.lua")

target("Doom93")
    set_kind("binary")
    add_deps("EngineSquared")
    add_files("src/**.cpp")
    add_includedirs("src/")
    add_packages("entt", "glm", "spdlog", "fmt", "tinyobjloader")
    set_rundir("$(projectdir)")
    if is_plat("macosx") then
        add_ldflags("-ObjC")
        add_frameworks("Carbon")
    end
target_end()
