#include "game/system/ComputeDamage/ComputeDamage.hpp"
#include <gtest/gtest.h>

using game::component::DamageType;
using game::system::ComputeDamage;

TEST(ComputeDamage, TypeModifiers)
{
    EXPECT_FLOAT_EQ(ComputeDamage(100.f, DamageType::Hitscan), 100.f);
    EXPECT_FLOAT_EQ(ComputeDamage(100.f, DamageType::Projectile), 100.f);
    EXPECT_FLOAT_EQ(ComputeDamage(100.f, DamageType::Explosion), 150.f);
    EXPECT_FLOAT_EQ(ComputeDamage(100.f, DamageType::Environment), 100.f);
}
