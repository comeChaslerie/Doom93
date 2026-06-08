#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "fmt/format.h"
#include "game/component/Health.hpp"
#include "game/loader/WadLoader.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "scheduler/Startup.hpp"
#include <exception>

using namespace game::component;

namespace {
void LoadWad(const std::string &path)
{
    try
    {
        const game::loader::LumpData wad = game::loader::WadLoader(path);
    }
    catch (const std::exception &error)
    {
        Log::Error(fmt::format("Echec du chargement de '{}' : {}", path, error.what()));
    }
}
} // namespace

int main()
{
    Engine::Core core;

    LoadWad("freedoom1.wad");

    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        auto player = core.CreateEntity();
        player.AddComponent<game::component::Health>();
        player.AddComponent<Object::Component::Transform>(glm::vec3(0.75f, 1.0f, -2.5f), glm::vec3(1.0f),
                                                          glm::quat(glm::vec3(glm::radians(20.0f), 0.f, 0.f)));
        player.AddComponent<Object::Component::Camera>();
    });
    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin>();
    core.Run();
    return 0;
}
