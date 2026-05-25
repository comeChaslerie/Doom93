#pragma once

#include "game/component/Damage/DamageType.hpp"

namespace game::component {
struct DamageEvent {
    float amount;
    DamageType type;
};
} // namespace game::component
