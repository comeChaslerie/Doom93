#include "game/system/ApplyDamage/ApplyDamage.hpp"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Health.hpp"
#include "game/system/ComputeDamage/ComputeDamage.hpp"
#include <algorithm>

void game::system::ApplyDamage(Engine::Core &core)
{
    auto &registery = core.GetRegistry();

    registery.view<component::Health, component::DamageEvent>().each(
        [](component::Health &health, component::DamageEvent &event) {
            health.current -= ComputeDamage(event.amount, event.type);
            health.current = std::max(health.current, 0.f);
        });
    registery.clear<component::DamageEvent>();
}
