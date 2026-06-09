#include "game/system/Movement/MovementSystem.hpp"
#include "component/Transform.hpp"
#include "entt/entity/fwd.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "resource/Time.hpp"

void game::system::MovementSystem(Engine::Core &core)
{
    auto elapsed = core.GetResource<Engine::Resource::Time>()._elapsedTime;
    for (auto &&[entity, transform, velocity] :
         core.GetRegistry().view<Object::Component::Transform, component::Velocity>().each())
    {
        transform.SetPosition(transform.GetPosition() + (velocity.movement * elapsed));
    }
}
