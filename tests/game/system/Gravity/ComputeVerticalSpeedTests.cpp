#include "game/system/Gravity/ComputeVerticalSpeed.hpp"
#include <gtest/gtest.h>

using game::system::ComputeVerticalSpeed;

TEST(ComputeVerticalSpeed, StaysStillOnTheFloor) { EXPECT_FLOAT_EQ(ComputeVerticalSpeed(0.f, true), 0.f); }

TEST(ComputeVerticalSpeed, LandingStopsTheFall) { EXPECT_FLOAT_EQ(ComputeVerticalSpeed(-12.f, true), 0.f); }

TEST(ComputeVerticalSpeed, LeavingTheFloorStartsAtTwiceTheGravity)
{
    EXPECT_FLOAT_EQ(ComputeVerticalSpeed(0.f, false), -2.f);
}

TEST(ComputeVerticalSpeed, FallingGainsOneUnitPerTic) { EXPECT_FLOAT_EQ(ComputeVerticalSpeed(-5.f, false), -6.f); }

TEST(ComputeVerticalSpeed, FallsLikeDoom)
{
    // Dans Doom, apres 15 tics en l'air on a parcouru 2 + 3 + ... + 16 = 135 unites.
    float speed = 0.f;
    float distance = 0.f;
    for (int tic = 0; tic < 15; ++tic)
    {
        speed = ComputeVerticalSpeed(speed, false);
        distance -= speed;
    }
    EXPECT_FLOAT_EQ(speed, -16.f);
    EXPECT_FLOAT_EQ(distance, 135.f);
}
