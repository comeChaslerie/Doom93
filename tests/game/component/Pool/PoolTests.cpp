#include "game/component/Pool/Pool.hpp"
#include <gtest/gtest.h>

using game::component::Pool;

namespace {
struct Bullet {
    int dmg = 7;
};
} // namespace

TEST(Pool, ConstructionFillsCapacity)
{
    Pool<int> p(4);

    EXPECT_EQ(p.Capacity(), 4U);
    EXPECT_EQ(p.Available(), 4U);
}

TEST(Pool, AcquireDecrementsAvailable)
{
    Pool<int> p(4);

    int *obj = p.Acquire();
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(p.Available(), 3U);
    EXPECT_EQ(p.Capacity(), 4U); // la capacite ne bouge pas, seul le stock libre baisse
}

TEST(Pool, ReleaseReturnsObjectToPool)
{
    Pool<int> p(2);

    int *obj = p.Acquire();
    EXPECT_EQ(p.Available(), 1U);
    p.Release(obj);
    EXPECT_EQ(p.Available(), 2U);
}

TEST(Pool, ExhaustionReturnsNullptr)
{
    Pool<int> p(2);

    EXPECT_NE(p.Acquire(), nullptr);
    EXPECT_NE(p.Acquire(), nullptr);
    EXPECT_EQ(p.Acquire(), nullptr); // pool epuise -> nullptr, pas de crash
    EXPECT_EQ(p.Available(), 0U);
}

TEST(Pool, WorksForAnyType)
{
    Pool<Bullet> p(3); // meme code generique pour un type non numerique

    Bullet *b = p.Acquire();
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->dmg, 7);
    EXPECT_EQ(p.Available(), 2U);
}
