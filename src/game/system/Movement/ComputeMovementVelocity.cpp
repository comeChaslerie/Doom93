#include "game/system/Movement/ComputeMovementVelocity.hpp"
#include "component/Transform.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/ressources/Keys/Keys.hpp"
#include "glm/geometric.hpp"

namespace {
constexpr float epsilon = 1e-6f;
}

void game::system::ComputeMovementVelocity(component::Velocity &velocity, game::ressource::Keys &keys,
                                           Object::Component::Transform &transform)
{
    auto forward = transform.GetForwardVector();
    auto right = transform.GetRightVector();
    velocity.movement = {0.f, 0.f, 0.f};
    if (keys.GetKeyState(game::component::KeysType::UP_Key))
        velocity.movement += forward;
    if (keys.GetKeyState(game::component::KeysType::DOWN_Key))
        velocity.movement -= forward;
    if (keys.GetKeyState(game::component::KeysType::LEFT_Key))
        velocity.movement -= right;
    if (keys.GetKeyState(game::component::KeysType::RIGHT_Key))
        velocity.movement += right;
    if (velocity.movement != glm::vec3(0.f))
        velocity.movement = glm::normalize(velocity.movement);
    else
        velocity.movement = {0.f, 0.f, 0.f};
}
