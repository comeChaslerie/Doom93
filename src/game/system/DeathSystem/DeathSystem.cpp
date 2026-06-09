#include "game/system/DeathSystem/DeathSystem.hpp"
#include "entt/entity/fwd.hpp"
#include "game/component/Death/DeathEvent.hpp"
#include "game/component/Health.hpp"
#include "game/component/Tags/Dead.hpp"
#include "resource/EventManager.hpp"

namespace game::system {
void DeathSystem(Engine::Core &core)
{
    auto &events = core.GetResource<Event::Resource::EventManager>();
    auto &registery = core.GetRegistry();
    for (auto &&[entity, health] : registery.view<component::Health>(entt::exclude<component::Dead>).each())
    {
        if (health.hp.IsEmpty())
        {
            events.PushEvent(component::DeathEvent{entity});
            registery.emplace_or_replace<component::Dead>(entity);
        }
    }
}
} // namespace game::system
