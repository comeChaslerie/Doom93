#include "game/system/Movement/ApplyMovement.hpp"
#include "component/CharacterController.hpp"
#include "component/Transform.hpp"
#include "entt/entity/fwd.hpp"
#include "game/component/Speed.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "resource/Time.hpp"

void game::system::ApplyMovement(Engine::Core &core)
{
    auto elapsed = core.GetResource<Engine::Resource::Time>()._elapsedTime;
    auto &registry = core.GetRegistry();

    for (auto &&[entity, transform, velocity, speed] :
         registry.view<Object::Component::Transform, component::Velocity, component::Speed>().each())
    {
        // The physics plugin rewrites the Transform of a character controller every fixed tick, so its
        // movement has to go through the controller's velocity. The vertical axis is left to gravity.
        if (auto *controller = registry.try_get<Physics::Component::CharacterController>(entity))
        {
            const glm::vec3 movement = velocity.movement * speed.speed.Current();
            controller->linearVelocity.x = movement.x;
            controller->linearVelocity.z = movement.z;
            continue;
        }
        transform.SetPosition(transform.GetPosition() + velocity.movement * elapsed * speed.speed.Current());
    }
}
