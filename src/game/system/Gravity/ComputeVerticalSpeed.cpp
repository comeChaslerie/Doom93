#include "game/system/Gravity/ComputeVerticalSpeed.hpp"

float game::system::ComputeVerticalSpeed(float verticalSpeed, bool onFloor)
{
    if (onFloor)
        return 0.f;
    if (verticalSpeed == 0.f)
        return -2.f * GRAVITY;
    return verticalSpeed - GRAVITY;
}
