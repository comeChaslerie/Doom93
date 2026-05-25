#include "game/system/ComputeDamage/ComputeDamage.hpp"
#include "game/component/Damage/DamageType.hpp"
#include <algorithm>

using namespace game::component;

namespace {
constexpr float HITSCAN_MULT = 1.f;
constexpr float PROJECTILE_MULT = 1.f;
constexpr float EXPLOSION_MULT = 1.5f;
constexpr float ENVIRONMENT_MULT = 1.f;
}

float game::system::ComputeDamage(float base, DamageType type)
{
    switch (type) {
        case DamageType::Hitscan:
            return base * HITSCAN_MULT;
        case DamageType::Projectile:
            return base * PROJECTILE_MULT;
        case DamageType::Explosion:
            return base * EXPLOSION_MULT;
        case DamageType::Environment:
            return base * ENVIRONMENT_MULT;
    }
    return base;
}

float game::system::ComputeDamage(float base, DamageType type,
    float distance, float falloff)
{
    float damages = ComputeDamage(base, type);
    float factor = std::max(0.f, 1.f - distance * falloff);
    return damages * factor;
}
