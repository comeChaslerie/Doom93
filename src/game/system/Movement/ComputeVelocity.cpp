#include "game/system/Movement/ComputeVelocity.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cmath>

glm::vec3 game::system::ComputeVelocity(double angle)
{
    glm::vec3 velocity = {cos(angle), sin(angle), 0};

    return velocity;
}
