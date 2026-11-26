#include "game/plugin/Movement/MovementPlugin.hpp"
#include "GLFW/glfw3.h"
#include "game/component/Keys/KeysType.hpp"
#include "game/ressources/Keys/Keys.hpp"
#include "game/system/Gravity/ApplyGravity.hpp"
#include "game/system/Movement/ApplyMovement.hpp"
#include "game/system/Movement/ComputeMovement.hpp"
#include "game/system/Movement/UpdateKeysState.hpp"
#include "game/system/StepUp/ApplyStepUp.hpp"
#include "plugin/PluginInput.hpp"
#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Update.hpp"

namespace game::plugin {
void MovementPlugin::Bind()
{
    auto &keys = RegisterResource(game::ressource::Keys{});
    keys.SetKeyCode(component::KeysType::UP_Key, GLFW_KEY_W);
    keys.SetKeyCode(component::KeysType::DOWN_Key, GLFW_KEY_S);
    keys.SetKeyCode(component::KeysType::LEFT_Key, GLFW_KEY_A);
    keys.SetKeyCode(component::KeysType::RIGHT_Key, GLFW_KEY_D);
    RequirePlugins<Input::Plugin>();
    RegisterSystems<Engine::Scheduler::Update>(system::UpdateKeysState, system::ComputeMovement,
                                               game::system::ApplyMovement);
    // In this order, before the physics plugin's own update moves the characters: gravity reads the floor of the last
    // tic, then ApplyStepUp replaces it with the floor of this one.
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(system::ApplyGravity, system::ApplyStepUp);
}
} // namespace game::plugin
