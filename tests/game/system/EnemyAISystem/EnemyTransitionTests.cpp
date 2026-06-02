#include "game/system/EnemyAISystem/EnemyTransition.hpp"
#include <gtest/gtest.h>

using game::component::EnemyState;
using game::system::GetNextState;
using game::system::Perception;

// --- Transversales (prioritaires, depuis n'importe quel etat) ---

TEST(EnemyTransition, DeadOverridesEverything)
{
    Perception p;
    p.isDead = true;
    p.isTouch = true; // meme si touche, la mort prime

    EXPECT_EQ(GetNextState(EnemyState::Idle, p), EnemyState::Dead);
    EXPECT_EQ(GetNextState(EnemyState::Chase, p), EnemyState::Dead);
    EXPECT_EQ(GetNextState(EnemyState::Attack, p), EnemyState::Dead);
    EXPECT_EQ(GetNextState(EnemyState::Pain, p), EnemyState::Dead);
}

TEST(EnemyTransition, TouchGoesToPainWhenAlive)
{
    Perception p;
    p.isTouch = true;

    EXPECT_EQ(GetNextState(EnemyState::Idle, p), EnemyState::Pain);
    EXPECT_EQ(GetNextState(EnemyState::Chase, p), EnemyState::Pain);
    EXPECT_EQ(GetNextState(EnemyState::Attack, p), EnemyState::Pain);
}

// --- Idle ---

TEST(EnemyTransition, IdleStaysIdleWithoutStimulus)
{
    Perception p;
    EXPECT_EQ(GetNextState(EnemyState::Idle, p), EnemyState::Idle);
}

TEST(EnemyTransition, IdleSeesPlayerChases)
{
    Perception p;
    p.isVisible = true;
    EXPECT_EQ(GetNextState(EnemyState::Idle, p), EnemyState::Chase);
}

TEST(EnemyTransition, IdleInRangeAttacks)
{
    Perception p;
    p.isReachable = true;
    EXPECT_EQ(GetNextState(EnemyState::Idle, p), EnemyState::Attack);
}

// --- Chase ---

TEST(EnemyTransition, ChaseStaysChaseOutOfRange)
{
    Perception p;
    p.isVisible = true; // voit mais pas a portee
    EXPECT_EQ(GetNextState(EnemyState::Chase, p), EnemyState::Chase);
}

TEST(EnemyTransition, ChaseInRangeAttacks)
{
    Perception p;
    p.isReachable = true;
    EXPECT_EQ(GetNextState(EnemyState::Chase, p), EnemyState::Attack);
}

// --- Attack ---

TEST(EnemyTransition, AttackStaysAttackUntilTimerEnds)
{
    Perception p;
    EXPECT_EQ(GetNextState(EnemyState::Attack, p), EnemyState::Attack);
}

TEST(EnemyTransition, AttackBackToChaseWhenTimerEnds)
{
    Perception p;
    p.isAttackFinish = true;
    EXPECT_EQ(GetNextState(EnemyState::Attack, p), EnemyState::Chase);
}

// --- Pain ---

TEST(EnemyTransition, PainStaysPainWhileTimerRunning)
{
    Perception p;
    EXPECT_EQ(GetNextState(EnemyState::Pain, p), EnemyState::Pain);
}

TEST(EnemyTransition, PainRecoversToChaseWhenStillSeesPlayer)
{
    Perception p;
    p.isPainFinish = true;
    p.isVisible = true;
    EXPECT_EQ(GetNextState(EnemyState::Pain, p), EnemyState::Chase);
}

TEST(EnemyTransition, PainRecoversToIdleWhenPlayerLost)
{
    Perception p;
    p.isPainFinish = true;
    EXPECT_EQ(GetNextState(EnemyState::Pain, p), EnemyState::Idle);
}
