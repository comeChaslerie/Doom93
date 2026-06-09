#include "game/plugin/Movement/MovementPlugin.hpp"
#include "game/system/Movement/MovementSystem.hpp"
#include "scheduler/Update.hpp"

namespace game::plugin {
void MovementPlugin::Bind()
{
    RegisterSystems<Engine::Scheduler::Update>(game::system::MovementSystem);
}
} // namespace game::plugin
