#include "game/system/DeathSystem/DeathSystem.hpp"
#include "game/component/Death/DeathEvent.hpp"
#include "game/component/Health.hpp"
#include "resource/EventManager.hpp"

namespace game::system {
void DeathSystem(Engine::Core &core)
{
    auto &events = core.GetResource<Event::Resource::EventManager>();
    for (auto &&[entity, health] : core.GetRegistry().view<component::Health>().each())
    {
        if (health.hp.IsEmpty())
            events.PushEvent(component::DeathEvent{entity});
    }
}
} // namespace game::system
