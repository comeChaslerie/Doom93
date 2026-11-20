#include "game/system/Gravity/ApplyGravity.hpp"
#include "component/CharacterController.hpp"
#include "component/CharacterControllerInternal.hpp"
#include "game/Tic.hpp"
#include "game/system/Gravity/ComputeVerticalSpeed.hpp"

void game::system::ApplyGravity(Engine::Core &core)
{
    auto view = core.GetRegistry()
                    .view<Physics::Component::CharacterController, Physics::Component::CharacterControllerInternal>();

    for (auto &&[entity, controller, character] : view.each())
    {
        if (!character.IsValid())
            continue;
        // Jolt works in units per second: with a fixed step of one tic, a speed of N units per tic handed over as
        // N * RATE moves the character by exactly N units during the step.
        const float speed =
            ComputeVerticalSpeed(controller.linearVelocity.y / tic::RATE, character.character->IsSupported());
        controller.linearVelocity.y = speed * tic::RATE;
    }
}
