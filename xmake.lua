---@diagnostic disable: undefined-global, undefined-field

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

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name ~= "main" then
        target(name)
        set_kind("binary")
        set_group("test")
        set_default(false)
        set_languages("c++20")
        add_packages("gtest")
        add_links("gtest")
        add_tests("default")
        add_includedirs("src/")
        add_files(file)
        add_files("tests/main.cpp")
        add_files("src/game/system/ComputeDamage/ComputeDamage.cpp")
        target_end()
    end
end
