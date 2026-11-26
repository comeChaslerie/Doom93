#include "game/system/StepUp/ApplyStepUp.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/CharacterController.hpp"
#include "component/CharacterControllerInternal.hpp"
#include "game/Tic.hpp"
#include "game/component/Floor.hpp"
#include "game/loader/LumpsData.hpp"
#include "game/system/StepUp/ComputeFloorHeight.hpp"
#include <glm/vec2.hpp>

void game::system::ApplyStepUp(Engine::Core &core)
{
    // The game only plays the first level of the WAD (see main.cpp).
    const game::loader::Level &level = core.GetResource<game::loader::LumpData>().levels.at(0);
    auto view = core.GetRegistry()
                    .view<Physics::Component::CharacterController, Physics::Component::CharacterControllerInternal,
                          Physics::Component::CapsuleCollider, component::Floor>();

    for (auto &&[entity, controller, character, capsule, floor] : view.each())
    {
        if (!character.IsValid())
            continue;
        // The capsule is centered on the character's position. World x and z are the map's x and y.
        const JPH::RVec3 position = character.character->GetPosition();
        const float halfHeight = capsule.GetTotalHeight() / 2.f;
        const Body body{
            .position = {static_cast<float>(position.GetX()), static_cast<float>(position.GetZ())},
            .feet = static_cast<float>(position.GetY()) - halfHeight,
            .radius = capsule.radius,
            .height = capsule.GetTotalHeight(),
        };
        // Jolt works in units per second, and one fixed update lasts one tic.
        const glm::vec2 move = glm::vec2{controller.linearVelocity.x, controller.linearVelocity.z} / tic::RATE;
        const float fall = controller.linearVelocity.y / tic::RATE;

        floor.height = ComputeFloorHeight(level, body, move);
        // A body below its floor is put back on it at once: that is the step up. One that would fall through it this
        // tic stops on it, and its fall ends there (z = floorz and momz = 0 in P_ZMovement).
        if (body.feet < floor.height || (fall < 0.f && body.feet + fall <= floor.height))
        {
            character.character->SetPosition(JPH::RVec3{position.GetX(), floor.height + halfHeight, position.GetZ()});
            controller.linearVelocity.y = 0.f;
        }
    }
}
