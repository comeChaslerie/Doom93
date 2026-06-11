#pragma once

#include "glm/fwd.hpp"

namespace game::system {
glm::quat ComputeMouseMovement(const glm::quat &current, float mouseDelta);
}
