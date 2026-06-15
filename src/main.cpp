#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Mesh.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "fmt/format.h"
#include "game/component/MouseMovement/LastMousePos.hpp"
#include "game/component/MouseMovement/Sensibility.hpp"
#include "game/component/Speed.hpp"
#include "game/component/Tags/Player.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/loader/LumpsData.hpp"
#include "game/loader/WadLoader.hpp"
#include "game/plugin/MouseMovement/MouseMovementPlugin.hpp"
#include "game/plugin/Movement/MovementPlugin.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "glm/fwd.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"
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
        auto window = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        double posx = 0;
        glfwGetCursorPos(window, &(posx), nullptr);

        const auto &level = core.GetResource<game::loader::LumpData>().levels.at(0);

        // Spawn = Thing de type 1 (player 1 start), hauteur d'oeil ~41 (Doom).
        glm::vec3 spawn(0.f, 41.f, 0.f);
        for (const auto &thing : level.things)
            if (thing.type == 1)
                spawn = glm::vec3(thing.position.x, 41.f, thing.position.y);

        auto player = core.CreateEntity();
        player.AddComponent<Object::Component::Transform>(spawn);
        player.AddComponent<Object::Component::Camera>();
        player.AddComponent<game::component::LastMousePos>(static_cast<float>(posx));
        player.AddComponent<game::component::Sensibility>();
        player.AddComponent<game::component::Player>();
        player.AddComponent<game::component::Velocity>();
        player.AddComponent<game::component::Speed>();

        // Geometrie du niveau : les murs (linedefs -> quads).
        auto walls = core.CreateEntity();
        walls.AddComponent<Object::Component::Transform>();
        walls.AddComponent<Object::Component::Mesh>(game::system::BuildWalls(level));
    });
    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, game::plugin::MouseMovementPlugin,
                    game::plugin::MovementPlugin>();
    core.Run();
    return 0;
}
