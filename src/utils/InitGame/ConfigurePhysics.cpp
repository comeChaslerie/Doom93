#include "utils/InitGame/ConfigurePhysics.hpp"
#include "game/Tic.hpp"
#include "resource/PhysicsManager.hpp"
#include "scheduler/FixedTimeUpdate.hpp"

namespace {
/// @brief Jolt recommends one collision step per 1/60 s, so a 1/35 s step needs two.
constexpr int COLLISION_STEPS = 2;
} // namespace

void utils::InitGame::ConfigurePhysics(Engine::Core &core)
{
    core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().SetTickRate(game::tic::DURATION);
    core.GetResource<Physics::Resource::PhysicsManager>().SetCollisionSteps(COLLISION_STEPS);
}
