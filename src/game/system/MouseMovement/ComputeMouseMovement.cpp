#include "game/system/MouseMovement/ComputeMouseMovement.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include <glm/gtc/quaternion.hpp>

namespace {
constexpr glm::vec3 axisY = glm::vec3(0.f, 1.f, 0.f);
}

glm::quat game::system::ComputeMouseMovement(const glm::quat &current, float mouseDelta)
{
    glm::quat angle = glm::angleAxis(mouseDelta, axisY);
    glm::quat result = glm::normalize(angle * current);
    return result;
}
