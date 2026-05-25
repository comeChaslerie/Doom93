#include "game/system/ApplyDamage.hpp"
#include "game/component/DamageEvent.hpp"
#include "game/component/Health.hpp"
#include <algorithm>

void game::system::ApplyDamage(Engine::Core &core) {
    auto &registery = core.GetRegistry();

    registery.view<component::Health, component::DamageEvent>().each([](component::Health &health, component::DamageEvent &event) {
        health.current -= event.amount;
        health.current = std::max(health.current, 0.f);
    });
    registery.clear<component::DamageEvent>();
}
