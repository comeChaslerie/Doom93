#include "game/component/Cooldown/Cooldown.hpp"
#include <gtest/gtest.h>

using game::component::Cooldown;

TEST(Cooldown, FreshCooldownIsReady)
{
    Cooldown cd(0.5f);

    EXPECT_TRUE(cd.Ready()); // une arme neuve peut tirer tout de suite
    EXPECT_FLOAT_EQ(cd.Remaining().Value(), 0.f);
    EXPECT_FLOAT_EQ(cd.Duration().Value(), 0.5f);
}

TEST(Cooldown, TriggerArmsTheCooldown)
{
    Cooldown cd(0.5f);

    cd.Trigger();
    EXPECT_FALSE(cd.Ready());
    EXPECT_FLOAT_EQ(cd.Remaining().Value(), 0.5f); // remaining = duration
}

TEST(Cooldown, TickReducesRemaining)
{
    Cooldown cd(0.5f);

    cd.Trigger();
    cd.Tick(0.2f);
    EXPECT_FALSE(cd.Ready());
    EXPECT_FLOAT_EQ(cd.Remaining().Value(), 0.3f);
}

TEST(Cooldown, TickClampsAtZeroWhenDtExceedsRemaining)
{
    Cooldown cd(0.5f);

    cd.Trigger();
    cd.Tick(1.f); // dt > remaining : ne doit PAS devenir negatif
    EXPECT_TRUE(cd.Ready());
    EXPECT_FLOAT_EQ(cd.Remaining().Value(), 0.f);
}

TEST(Cooldown, RetriggerRefreshesRemaining)
{
    Cooldown cd(0.5f);

    cd.Trigger();
    cd.Tick(0.3f);  // remaining = 0.2
    cd.Trigger();   // re-trigger en plein cooldown
    EXPECT_FALSE(cd.Ready());
    EXPECT_FLOAT_EQ(cd.Remaining().Value(), 0.5f); // reparti a duration
}

TEST(Cooldown, TickWhenAlreadyReadyStaysReady)
{
    Cooldown cd(0.5f);

    cd.Tick(1.f); // deja pret (remaining 0) -> reste 0, pas d'underflow
    EXPECT_TRUE(cd.Ready());
    EXPECT_FLOAT_EQ(cd.Remaining().Value(), 0.f);
}
