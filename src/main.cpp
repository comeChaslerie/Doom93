#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "fmt/format.h"
#include "game/loader/WadLoader.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "scheduler/Startup.hpp"
#include <exception>

namespace {
/// @brief try to load WAD file
/// @return true, or false if an error occurs and logs an error
bool LoadWad(Engine::Core &core, const std::string &path)
{
    try
    {
        core.RegisterResource(game::loader::WadLoader(path));
        return true;
    }
    catch (const std::exception &error)
    {
        Log::Error(fmt::format("Loading of '{}' failed : {}", path, error.what()));
        return false;
    }
}
} // namespace

int main()
{
    Engine::Core core;

    if (!LoadWad(core, "freedoom/freedoom1.wad"))
        return 84;
    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        auto camera = core.CreateEntity();
        camera.AddComponent<Object::Component::Transform>(glm::vec3(0.75f, 1.0f, -2.5f), glm::vec3(1.0f),
                                                          glm::quat(glm::vec3(glm::radians(20.0f), 0.f, 0.f)));
        camera.AddComponent<Object::Component::Camera>();
    });
    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin>();
    core.Run();
    return 0;
}
