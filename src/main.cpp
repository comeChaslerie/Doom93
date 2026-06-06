#include "Logger.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "fmt/format.h"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Damage/DamageType.hpp"
#include "game/component/Death/DeathEvent.hpp"
#include "game/component/Health.hpp"
#include "game/loader/WadLoader.hpp"
#include "game/plugin/Combat/CombatPlugin.hpp"
#include "game/system/DeathSystem/DeathSystem.hpp"
#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include "scheduler/Startup.hpp"
#include "scheduler/Update.hpp"

#include <exception>

using namespace game::component;

namespace {
void LoadWad(const std::string &path)
{
    try
    {
        const game::loader::LumpData wad = game::loader::WadLoader(path);
        Log::Info(fmt::format("WAD '{}' charge : {} niveaux, {} palettes, {} colormaps, {} pnames, {} textures, {} "
                              "flats, {} sprites/patches",
                              path, wad.levels.size(), wad.palettes.size(), wad.colormaps.size(), wad.pnames.size(),
                              wad.textures.size(), wad.flats.size(), wad.pictures.size()));
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
    int deaths = 0;

    LoadWad("freedoom/freedoom1.wad");

    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        auto entity = core.CreateEntity();
        auto &health = entity.AddComponent<Health>();
        entity.AddComponent<DamageEvent>(150.f, DamageType::Hitscan);
        Log::Info(fmt::format("Spawn : {}/{} PV", health.hp.Current(), health.hp.Max()));
    });
    core.AddPlugins<game::plugin::CombatPlugin>();
    core.AddPlugins<Event::Plugin>();
    auto &events = core.GetResource<Event::Resource::EventManager>();
    events.RegisterCallback<DeathEvent>(
        [](const DeathEvent &e) { Log::Info(fmt::format("DeathEvent: entity {} is dead", e.entity)); });
    events.RegisterCallback<DeathEvent>([&deaths](const DeathEvent &e) { ++deaths; });
    core.RegisterSystem<Engine::Scheduler::Update>([](Engine::Core &core) {
        core.GetRegistry().view<Health>().each(
            [](Health &health) { Log::Info(fmt::format("After : {}/{} PV", health.hp.Current(), health.hp.Max())); });
        core.Stop();
    });
    core.RegisterSystem<Engine::Scheduler::Update>(game::system::DeathSystem);
    core.Run();
    Log::Info(fmt::format("Total deaths counted: {}", deaths));
    return 0;
}
