#include "game/component/Stat/Stat.hpp"
#include <gtest/gtest.h>

using game::component::Stat;

TEST(Stat, Construction)
{
    Stat s(100.f);

    EXPECT_FLOAT_EQ(s.Current(), 100.f);
    EXPECT_FLOAT_EQ(s.Max(), 100.f);
    EXPECT_FLOAT_EQ(s.Ratio(), 1.f);
    EXPECT_FALSE(s.IsEmpty());
}

TEST(Stat, DamageClampsAtZero)
{
    Stat s(100.f);

    s.Damage(150.f); // plus que le max
    EXPECT_FLOAT_EQ(s.Current(), 0.f);
    EXPECT_TRUE(s.IsEmpty());
}

TEST(Stat, HealCapsAtMax)
{
    Stat s(100.f);

    s.Damage(40.f); // current = 60
    s.Heal(1000.f); // plafonne au max, pas 1060
    EXPECT_FLOAT_EQ(s.Current(), 100.f);
}

TEST(Stat, NegativeHealNeverBelowZero)
{
    Stat s(100.f);

    s.Damage(80.f); // current = 20
    s.Heal(-50.f);  // 20 - 50 = -30 -> clampe a 0
    EXPECT_FLOAT_EQ(s.Current(), 0.f);
    EXPECT_TRUE(s.IsEmpty());
}

TEST(Stat, SetMaxLowersAndReclamps)
{
    Stat s(100.f);

    s.SetMax(50.f); // baisse le max sous current, sans refill
    EXPECT_FLOAT_EQ(s.Max(), 50.f);
    EXPECT_FLOAT_EQ(s.Current(), 50.f);
}

TEST(Stat, SetMaxWithRefill)
{
    Stat s(100.f);

    s.Damage(100.f);       // current = 0
    s.SetMax(200.f, true); // refill -> current = max = 200
    EXPECT_FLOAT_EQ(s.Max(), 200.f);
    EXPECT_FLOAT_EQ(s.Current(), 200.f);
}

TEST(Stat, NegativeMaxIsClampedNoNaN)
{
    Stat s(-10.f); // max negatif -> borne a 0

    EXPECT_FLOAT_EQ(s.Max(), 0.f);
    EXPECT_FLOAT_EQ(s.Current(), 0.f);
    EXPECT_TRUE(s.IsEmpty());
    EXPECT_FLOAT_EQ(s.Ratio(), 0.f); // surtout pas NaN (pas de division par 0)
}

TEST(Stat, RatioHalf)
{
    Stat s(100.f);

    s.Damage(50.f);
    EXPECT_FLOAT_EQ(s.Ratio(), 0.5f);
}
