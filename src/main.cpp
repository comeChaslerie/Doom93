#include "Logger.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "fmt/format.h"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Damage/DamageType.hpp"
#include "game/component/Death/DeathEvent.hpp"
#include "game/component/Health.hpp"
#include "game/system/DeathSystem/DeathSystem.hpp"
#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include "scheduler/Startup.hpp"
#include "scheduler/Update.hpp"
#include "game/plugin/Combat/CombatPlugin.hpp"

using namespace game::component;

int main()
{
    Engine::Core core;
    int deaths = 0;

    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        auto entity = core.CreateEntity();
        auto &health = entity.AddComponent<Health>();
        entity.AddComponent<DamageEvent>(150.f, DamageType::Hitscan);
        Log::Info(fmt::format("Spawn : {}/{} PV", health.hp.Current(), health.hp.Max()));
    });
    core.AddPlugins<game::plugin::CombatPlugin>();
    core.AddPlugins<Event::Plugin>();
    auto &events = core.GetResource<Event::Resource::EventManager>();
    events.RegisterCallback<DeathEvent>([](const DeathEvent &e) {
        Log::Info(fmt::format("DeathEvent: entity {} is dead", e.entity));
    });
    events.RegisterCallback<DeathEvent>([&deaths](const DeathEvent &e) {
        ++deaths;
    });
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
