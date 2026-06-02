#include "game/system/EnemyAISystem/EnemyAISystem.hpp"
#include "game/component/EnemyState/AIState.hpp"
#include "game/system/EnemyAISystem/EnemyTransition.hpp"

namespace game::system {
void EnemyAISystem(Engine::Core &core)
{
    for (auto &&[entity, ai, perception] : core.GetRegistry().view<component::AIState, Perception>().each())
    {
        ai.state = GetNextState(ai.state, perception);
    }
}
} // namespace game::system
