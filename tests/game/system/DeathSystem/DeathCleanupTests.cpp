#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "game/component/Death/Dead.hpp"
#include "game/component/Health.hpp"
#include "game/system/DeathSystem/DeathCleanup.hpp"
#include <gtest/gtest.h>

using game::component::Dead;
using game::component::Health;
using game::system::DeathCleanup;

TEST(DeathCleanup, DestroysTaggedEntity)
{
    Engine::Core core;
    auto entity = core.CreateEntity();
    entity.AddComponent<Health>();
    entity.AddComponent<Dead>();
    const Engine::EntityId id = entity.Id();

    DeathCleanup(core);

    EXPECT_FALSE(core.GetRegistry().valid(id));
}

TEST(DeathCleanup, KeepsUntaggedEntity)
{
    Engine::Core core;
    auto entity = core.CreateEntity();
    entity.AddComponent<Health>();
    const Engine::EntityId id = entity.Id();

    DeathCleanup(core);

    EXPECT_TRUE(core.GetRegistry().valid(id));
}

TEST(DeathCleanup, DestroysOnlyTaggedAmongMany)
{
    Engine::Core core;
    auto dead1 = core.CreateEntity();
    dead1.AddComponent<Dead>();
    auto alive = core.CreateEntity();
    alive.AddComponent<Health>();
    auto dead2 = core.CreateEntity();
    dead2.AddComponent<Dead>();

    const Engine::EntityId idDead1 = dead1.Id();
    const Engine::EntityId idAlive = alive.Id();
    const Engine::EntityId idDead2 = dead2.Id();

    DeathCleanup(core);

    EXPECT_FALSE(core.GetRegistry().valid(idDead1));
    EXPECT_TRUE(core.GetRegistry().valid(idAlive));
    EXPECT_FALSE(core.GetRegistry().valid(idDead2));
    EXPECT_EQ(core.GetRegistry().view<Dead>().size(), 0U);
}
