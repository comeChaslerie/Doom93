---@diagnostic disable: undefined-global, undefined-field

add_rules("mode.debug", "mode.release")
set_languages("c++20")

includes("EngineSquared/xmake.lua")

target("Doom93")
    set_kind("binary")
    add_deps("EngineSquared")
    add_files("src/**.cpp")
    add_includedirs("src/")
    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu",
        "lodepng")
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

-- Integration tests for ECS systems: they instantiate an Engine::Core, so they
-- need the EngineSquared dependency and several translation units. Declared
-- manually for the same reason as WadLoaderTests above.
local function engine_test(name, srcs)
    target(name)
        set_kind("binary")
        set_group("test")
        set_default(false)
        set_languages("c++20")
        add_deps("EngineSquared")
        add_packages("gtest", "entt", "glm", "spdlog", "fmt")
        add_links("gtest")
        add_tests("default")
        add_includedirs("src/")
        add_files("tests/main.cpp")
        for _, src in ipairs(srcs) do
            add_files(src)
        end
        set_rundir("$(projectdir)")
    target_end()
end

engine_test("ApplyDamageTests", {
    "tests/game/system/ApplyDamage/ApplyDamageTests.cpp",
    "src/game/system/ApplyDamage/ApplyDamage.cpp",
    "src/game/system/ComputeDamage/ComputeDamage.cpp",
})
engine_test("DeathSystemTests", {
    "tests/game/system/DeathSystem/DeathSystemTests.cpp",
    "src/game/system/DeathSystem/DeathSystem.cpp",
})
engine_test("DeathCleanupTests", {
    "tests/game/system/DeathSystem/DeathCleanupTests.cpp",
    "src/game/system/DeathSystem/DeathCleanup.cpp",
})
engine_test("HealthPipelineTests", {
    "tests/game/system/DeathSystem/HealthPipelineTests.cpp",
    "src/game/system/ApplyDamage/ApplyDamage.cpp",
    "src/game/system/ComputeDamage/ComputeDamage.cpp",
    "src/game/system/DeathSystem/DeathSystem.cpp",
    "src/game/system/DeathSystem/DeathCleanup.cpp",
})
engine_test("ApplyMovementTests", {
    "tests/game/system/Movement/ApplyMovementTests.cpp",
    "src/game/system/Movement/ApplyMovement.cpp",
})
engine_test("ComputeMovementVelocityTests", {
    "tests/game/system/Movement/ComputeMovementVelocityTests.cpp",
    "src/game/system/Movement/ComputeMovementVelocity.cpp",
})
engine_test("ComputeMouseMovementTests", {
    "tests/game/system/MouseMovement/ComputeMouseMovementTests.cpp",
    "src/game/system/MouseMovement/ComputeMouseMovement.cpp",
})

local manual_tests = {
    WadLoaderTests = true,
    ApplyDamageTests = true,
    DeathSystemTests = true,
    DeathCleanupTests = true,
    HealthPipelineTests = true,
    ApplyMovementTests = true,
    ComputeMovementVelocityTests = true,
    ComputeMouseMovementTests = true,
}

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name ~= "main" and not manual_tests[name] then
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
