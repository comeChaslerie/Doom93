#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Damage/DamageType.hpp"
#include "game/component/Health.hpp"
#include "game/system/ApplyDamage/ApplyDamage.hpp"
#include <gtest/gtest.h>

using game::component::DamageEvent;
using game::component::DamageType;
using game::component::Health;
using game::system::ApplyDamage;

TEST(ApplyDamage, LethalDamageEmptiesHealth)
{
    Engine::Core core;
    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>(); // 100 PV
    entity.AddComponent<DamageEvent>(150.f, DamageType::Hitscan);

    ApplyDamage(core);

    EXPECT_TRUE(health.hp.IsEmpty());
    EXPECT_FLOAT_EQ(health.hp.Current(), 0.f);
}

TEST(ApplyDamage, NonLethalDamageReducesHealth)
{
    Engine::Core core;
    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>();
    entity.AddComponent<DamageEvent>(30.f, DamageType::Hitscan);

    ApplyDamage(core);

    EXPECT_FALSE(health.hp.IsEmpty());
    EXPECT_FLOAT_EQ(health.hp.Current(), 70.f);
}

TEST(ApplyDamage, ConsumesDamageEvent)
{
    Engine::Core core;
    auto entity = core.CreateEntity();
    entity.AddComponent<Health>();
    entity.AddComponent<DamageEvent>(10.f, DamageType::Hitscan);

    ApplyDamage(core);

    // L'event est a usage unique : ApplyDamage le retire apres application.
    EXPECT_FALSE(core.GetRegistry().all_of<DamageEvent>(entity.Id()));
}

TEST(ApplyDamage, NoDamageEventLeavesHealthUntouched)
{
    Engine::Core core;
    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>();

    ApplyDamage(core);

    EXPECT_FLOAT_EQ(health.hp.Current(), 100.f);
}

TEST(ApplyDamage, ExplosionTypeAmplifiesDamage)
{
    Engine::Core core;
    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>();
    // Explosion x1.5 : 100 base -> 150 degats -> mort.
    entity.AddComponent<DamageEvent>(100.f, DamageType::Explosion);

    ApplyDamage(core);

    EXPECT_TRUE(health.hp.IsEmpty());
}
