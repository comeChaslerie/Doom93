#include "game/system/Movement/ComputeMovement.hpp"
#include "game/component/Tags/Player.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/ressources/Keys/Keys.hpp"
#include "game/system/Movement/ComputeMovementVelocity.hpp"

void game::system::ComputeMovement(Engine::Core &core)
{
    auto &keys = core.GetResource<ressource::Keys>();

    for (auto &&[entity, velocity] : core.GetRegistry().view<component::Velocity, component::Player>().each())
        ComputeMovementVelocity(velocity, keys);
}
