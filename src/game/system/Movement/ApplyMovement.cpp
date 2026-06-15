#include "game/system/Movement/ApplyMovement.hpp"
#include "component/Transform.hpp"
#include "entt/entity/fwd.hpp"
#include "game/component/Speed.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "resource/Time.hpp"

void game::system::ApplyMovement(Engine::Core &core)
{
    auto elapsed = core.GetResource<Engine::Resource::Time>()._elapsedTime;

    for (auto &&[entity, transform, velocity, speed] :
         core.GetRegistry().view<Object::Component::Transform, component::Velocity, component::Speed>().each())
    {
        transform.SetPosition(transform.GetPosition() + velocity.movement * elapsed * speed.speed.Current());
    }
}
