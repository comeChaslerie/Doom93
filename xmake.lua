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

-- Integration test for the WAD loader: needs several translation units, the glm
-- package, and the WAD fixture at runtime (rundir = project root). Declared
-- manually because the auto-derivation below only links a single source file.
target("WadLoaderTests")
    set_kind("binary")
    set_group("test")
    set_default(false)
    set_languages("c++20")
    add_packages("gtest", "glm")
    add_links("gtest")
    add_tests("default")
    add_includedirs("src/")
    add_files("tests/game/loader/WadLoaderTests.cpp")
    add_files("tests/main.cpp")
    add_files("src/game/loader/WadLoader.cpp")
    add_files("src/game/loader/WADReader.cpp")
    add_files("src/game/loader/WADReaderParse.cpp")
    add_files("src/utils/FileReader/FileReader.cpp")
    set_rundir("$(projectdir)")
target_end()

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name ~= "main" and name ~= "WadLoaderTests" then
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
        -- derive the source under test: tests/<path>/FooTests.cpp -> src/<path>/Foo.cpp
        local src = path.join("src", path.relative(path.directory(file), "tests"), name:gsub("Tests$", "") .. ".cpp")
        if os.isfile(src) then
            add_files(src)
        end
        target_end()
    end
end
