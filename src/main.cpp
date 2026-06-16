#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Material.hpp"
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
#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "glm/fwd.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
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
static constexpr glm::uvec2 textureSize = glm::uvec2(64, 64);
} // namespace

int main()
{
    Engine::Core core;

    if (!LoadWad(core, "freedoom/freedoom1.wad"))
        return 84;
    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        // Config Window
        auto window = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Get Ressources
        const auto &lumpData = core.GetResource<game::loader::LumpData>();
        const auto &level = lumpData.levels.at(0);
        auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
        auto &deviceContext = core.GetResource<Graphic::Resource::DeviceContext>();
        auto &queue = core.GetResource<Graphic::Resource::Queue>();

        // Spawn
        glm::vec3 spawn(0.f, 41.f, 0.f);
        for (const auto &thing : level.things)
            if (thing.type == 1)
                spawn = glm::vec3(thing.position.x, 41.f, thing.position.y);

        // Player && Camera
        auto player = core.CreateEntity();
        player.AddComponent<Object::Component::Transform>(spawn);
        auto &camera = player.AddComponent<Object::Component::Camera>();
        camera.farPlane = 10000.f;   // voit tout le niveau
        camera.nearPlane = 1.f;
        double posx = 0;
        glfwGetCursorPos(window, &(posx), nullptr);
        player.AddComponent<game::component::LastMousePos>(static_cast<float>(posx));
        player.AddComponent<game::component::Sensibility>();
        player.AddComponent<game::component::Player>();
        player.AddComponent<game::component::Velocity>();
        player.AddComponent<game::component::Speed>();

        // Murs
        auto walls = core.CreateEntity();
        walls.AddComponent<Object::Component::Transform>();
        walls.AddComponent<Object::Component::Mesh>(game::system::BuildWalls(level));

        // Sols & plafonds
        for (auto &&[tex, mesh] : game::system::BuildFloorCeil(level)) {
            auto it = std::find_if(lumpData.flats.begin(), lumpData.flats.end(),
                [&tex](const game::loader::Flat &f) { return f.name == tex; });
            if (it == lumpData.flats.end())
                continue;
            auto entity = core.CreateEntity();
            entity.AddComponent<Object::Component::Transform>();
            entity.AddComponent<Object::Component::Mesh>(mesh);
            const game::loader::Flat &flat = *it;
            auto texture = Graphic::Resource::Texture(deviceContext, queue,
            tex, textureSize, [&flat](glm::uvec2 pixel) { return flat.pixels[pixel.y * 64 + pixel.x]; });
            textureContainer.Add(flat.name, std::move(texture));
            Object::Component::Material mat;
            mat.diffuseTexName = flat.name;
            entity.AddComponent<Object::Component::Material>(std::move(mat));
        }
    });
    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, game::plugin::MouseMovementPlugin,
                    game::plugin::MovementPlugin>();
    core.Run();
    return 0;
}
