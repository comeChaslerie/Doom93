#include "utils/InitGame/ConfigurePhysics.hpp"
#include "game/Tic.hpp"
#include "resource/PhysicsManager.hpp"
#include "scheduler/FixedTimeUpdate.hpp"

namespace {
/// @brief Number of sub-steps each physics update is split into, run one after the other (collision detection, then
///   integration). Jolt recommends one per 1/60 s, so a 1/35 s tic needs two.
constexpr int COLLISION_STEPS = 2;
} // namespace

void utils::InitGame::ConfigurePhysics(Engine::Core &core)
{
    core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().SetTickRate(game::tic::DURATION);
    core.GetResource<Physics::Resource::PhysicsManager>().SetCollisionSteps(COLLISION_STEPS);
}
