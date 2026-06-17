#include "core/Core.hpp"
#include "game/loader/LumpsData.hpp"
#include "game/plugin/MouseMovement/MouseMovementPlugin.hpp"
#include "game/plugin/Movement/MovementPlugin.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "plugin/PluginPhysics.hpp"
#include "resource/TextureContainer.hpp"
#include "resource/Window.hpp"
#include "scheduler/Startup.hpp"
#include "utils/CreateEnvironment/CreateEnvironment.hpp"
#include "utils/InitGame/ConfigureWindow.hpp"
#include "utils/InitGame/InitPlayer.hpp"
#include "utils/InitGame/LoadWad.hpp"

namespace {
void Startup(Engine::Core &core)
{
    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        // Get Ressources
        const auto &lumpData = core.GetResource<game::loader::LumpData>();
        const auto &level = lumpData.levels.at(0);
        const auto &deviceContext = core.GetResource<Graphic::Resource::DeviceContext>();
        const auto &queue = core.GetResource<Graphic::Resource::Queue>();
        const auto window = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
        auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();

        // Helpers
        utils::InitGame::ConfigureWindow(core, window);
        utils::InitGame::InitPlayer(core, level, window);
        utils::CreateEnvironment::CreateWalls(core, level, lumpData, deviceContext, queue, textureContainer);
        utils::CreateEnvironment::CreateFloorAndCeiling(core, level, lumpData, deviceContext, queue, textureContainer);
    });
}
void AddPlugins(Engine::Core &core)
{
    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, game::plugin::MouseMovementPlugin,
        game::plugin::MovementPlugin, Physics::Plugin>();
}
} // namespace

int main()
{
    Engine::Core core;

    if (!utils::InitGame::LoadWad(core, "freedoom/freedoom1.wad"))
        return 84;
    AddPlugins(core);
    Startup(core);
    core.Run();
    return 0;
}
