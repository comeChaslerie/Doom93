#include "game/plugin/MouseMovement/MouseMovementPlugin.hpp"
#include "game/system/MouseMovement/ApplyMouseMovement.hpp"
#include "scheduler/Update.hpp"

namespace game::plugin {
void MouseMovementPlugin::Bind() { RegisterSystems<Engine::Scheduler::Update>(system::ApplyMouseMovement); }
} // namespace game::plugin
