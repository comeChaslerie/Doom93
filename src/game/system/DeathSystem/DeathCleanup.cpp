#include "game/system/DeathSystem/DeathCleanup.hpp"
#include "core/Core.hpp"
#include "entity/EntityId.hpp"
#include "game/component/Tags/Dead.hpp"
#include <vector>

namespace game::system {
void DeathCleanup(Engine::Core &core)
{
    std::vector<Engine::EntityId> entities;

    for (auto &&[entity] : core.GetRegistry().view<component::Dead>().each())
    {
        entities.push_back(entity);
    }
    for (Engine::EntityId entity : entities)
    {
        core.KillEntity(entity);
    }
}
} // namespace game::system
