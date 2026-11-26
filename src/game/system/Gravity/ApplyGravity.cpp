#include "game/system/Gravity/ApplyGravity.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/CharacterController.hpp"
#include "component/CharacterControllerInternal.hpp"
#include "game/Tic.hpp"
#include "game/component/Floor.hpp"
#include "game/system/Gravity/ComputeVerticalSpeed.hpp"

namespace {
/// @brief Gap under which a body counts as standing on its floor, in map units. The physics engine keeps the capsule a
///   little off the floor mesh (its character padding, 0.02 units).
constexpr float FLOOR_TOLERANCE = 0.1f;
} // namespace

void game::system::ApplyGravity(Engine::Core &core)
{
    auto view = core.GetRegistry()
                    .view<Physics::Component::CharacterController, Physics::Component::CharacterControllerInternal,
                          Physics::Component::CapsuleCollider, component::Floor>();

    for (auto &&[entity, controller, character, capsule, floor] : view.each())
    {
        if (!character.IsValid())
            continue;
        // Doom's floor rather than the physics engine's contacts: the square box keeps a body on a ledge that its round
        // capsule has already slid off. The Floor is still the one of the last tic, so this tells whether the body
        // ended it on its floor, which is what P_ZMovement checks before adding gravity.
        const float feet =
            static_cast<float>(character.character->GetPosition().GetY()) - capsule.GetTotalHeight() / 2.f;
        const bool onFloor = feet <= floor.height + FLOOR_TOLERANCE;
        // Jolt works in units per second: with a fixed step of one tic, a speed of N units per tic handed over as
        // N * RATE moves the character by exactly N units during the step.
        const float speed = ComputeVerticalSpeed(controller.linearVelocity.y / tic::RATE, onFloor);
        controller.linearVelocity.y = speed * tic::RATE;
    }
}
