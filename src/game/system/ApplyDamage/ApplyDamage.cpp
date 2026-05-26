#include "game/system/ApplyDamage/ApplyDamage.hpp"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Health.hpp"
#include "game/system/ComputeDamage/ComputeDamage.hpp"

void game::system::ApplyDamage(Engine::Core &core)
{
    auto &registery = core.GetRegistry();

    registery.view<component::Health, component::DamageEvent>().each(
        [](component::Health &health, component::DamageEvent &event) {
            health.hp.Damage(ComputeDamage(event.amount, event.type));
        });
    registery.clear<component::DamageEvent>();
}
