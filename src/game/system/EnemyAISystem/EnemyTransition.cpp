#include "game/system/EnemyAISystem/EnemyTransition.hpp"
#include "game/component/EnemyState/EnemyState.hpp"

namespace game::system {
component::EnemyState GetNextState(const component::EnemyState &state, const Perception &perception)
{
    if (perception.isDead)
        return component::EnemyState::Dead;
    if (perception.isTouch)
        return component::EnemyState::Pain;
    switch (state)
    {
    case component::EnemyState::Idle:
        if (perception.isReachable)
            return component::EnemyState::Attack;
        if (perception.isVisible)
            return component::EnemyState::Chase;
        return component::EnemyState::Idle;
    case component::EnemyState::Chase:
        if (perception.isReachable)
            return component::EnemyState::Attack;
        return component::EnemyState::Chase;
    case component::EnemyState::Attack:
        if (perception.isAttackFinish)
            return component::EnemyState::Chase;
        return component::EnemyState::Attack;
    case component::EnemyState::Pain:
        if (perception.isPainFinish)
        {
            if (perception.isVisible)
                return component::EnemyState::Chase;
            return component::EnemyState::Idle;
        }
        return component::EnemyState::Pain;
    case component::EnemyState::Dead: return component::EnemyState::Dead;
    }
    return state;
}
} // namespace game::system
