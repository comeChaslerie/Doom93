#include "game/system/Movement/ComputeMovementVelocity.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/ressources/Keys/Keys.hpp"
#include "game/system/Movement/ComputeVelocity.hpp"
#include "glm/geometric.hpp"
#include <numbers>

namespace {
constexpr float epsilon = 1e-6f;
}

void game::system::ComputeMovementVelocity(component::Velocity &velocity, game::ressource::Keys &keys)
{
    velocity.movement = {0.f, 0.f, 0.f};
    if (keys.GetKeyState(game::component::KeysType::UP_Key))
        velocity.movement += ComputeVelocity(0);
    if (keys.GetKeyState(game::component::KeysType::DOWN_Key))
        velocity.movement += ComputeVelocity(std::numbers::pi);
    if (keys.GetKeyState(game::component::KeysType::LEFT_Key))
        velocity.movement += ComputeVelocity(std::numbers::pi / 2.0);
    if (keys.GetKeyState(game::component::KeysType::RIGHT_Key))
        velocity.movement += ComputeVelocity(-std::numbers::pi / 2.0);
    if (glm::length(velocity.movement) > epsilon)
        velocity.movement = glm::normalize(velocity.movement);
    else
        velocity.movement = {0.f, 0.f, 0.f};
}
