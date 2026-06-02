#include "game/component/Damage/DamageModifier.hpp"
#include <gtest/gtest.h>
#include <memory.h>
#include <vector>

TEST(IDamageModifier, polymorphism)
{
    std::vector<std::unique_ptr<game::component::IDamageModifier>> mods;
    mods.push_back(std::make_unique<game::component::FlatBonus>(10.f));
    mods.push_back(std::make_unique<game::component::Multiplier>(2.f));
    float dmg = 25.f;
    for (const auto& m : mods)
        dmg = m->Modify(dmg);
    ASSERT_EQ(dmg, 70.f);
}
