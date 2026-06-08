#include "Logger.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "fmt/format.h"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Damage/DamageType.hpp"
#include "game/component/Death/DeathEvent.hpp"
#include "game/component/Health.hpp"
#include "game/loader/WadLoader.hpp"
#include "game/plugin/Health/HealthPlugin.hpp"
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

    LoadWad("freedoom1.wad");

    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        auto entity = core.CreateEntity();
        entity.AddComponent<Health>();
        entity.AddComponent<DamageEvent>(150.f, DamageType::Hitscan);
    });
    core.AddPlugins<game::plugin::HealthPlugin>();
    auto &events = core.GetResource<Event::Resource::EventManager>();events.RegisterCallback<DeathEvent>([&deaths](const DeathEvent &e) { ++deaths; });
    core.RegisterSystem<Engine::Scheduler::Update>([](Engine::Core &core) {
        core.Stop();
    });
    core.Run();
    Log::Info(fmt::format("Total deaths counted: {}", deaths));
    return 0;
}
