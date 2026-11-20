#include "game/system/Gravity/ComputeVerticalSpeed.hpp"

float game::system::ComputeVerticalSpeed(float verticalSpeed, bool onFloor)
{
    if (onFloor)
        return 0.f;
    if (verticalSpeed == 0.f)
        return FALL_START_SPEED;
    return verticalSpeed - GRAVITY;
}
