#pragma once

#include "game/component/Damage/DamageType.hpp"

namespace game::system {
[[nodiscard]] float ComputeDamage(float base, component::DamageType type);
[[nodiscard]] float ComputeDamage(float base, component::DamageType type,
    float distance, float falloff = 1.0f);
}
