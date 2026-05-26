#include "Logger.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "fmt/format.h"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Damage/DamageType.hpp"
#include "game/component/Health.hpp"
#include "game/system/ApplyDamage/ApplyDamage.hpp"
#include "scheduler/Startup.hpp"
#include "scheduler/Update.hpp"

using namespace game::component;

int main()
{
    Engine::Core core;

    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        Engine::Entity entity = core.CreateEntity();
        Health &health = entity.AddComponent<Health>();
        entity.AddComponent<DamageEvent>(25.f, DamageType::Hitscan);
        Log::Info(fmt::format("Spawn : {}/{} PV", health.hp.Current(), health.hp.Max()));
    });
    core.RegisterSystem<Engine::Scheduler::Update>(game::system::ApplyDamage);
    core.RegisterSystem<Engine::Scheduler::Update>([](Engine::Core &core) {
        core.GetRegistry().view<Health>().each(
            [](Health &health) { Log::Info(fmt::format("After : {}/{} PV", health.hp.Current(), health.hp.Max())); });
        core.Stop();
    });
    core.Run();
    return 0;
}
