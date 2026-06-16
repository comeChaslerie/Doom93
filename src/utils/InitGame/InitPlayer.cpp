#include "utils/InitGame/InitPlayer.hpp"
#include "GLFW/glfw3.h"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "game/component/MouseMovement/LastMousePos.hpp"
#include "game/component/MouseMovement/Sensibility.hpp"
#include "game/component/Speed.hpp"
#include "game/component/Tags/Player.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/loader/LumpsData.hpp"

namespace {
/// @brief Set Player spawn
glm::vec3 SetSpawn(const game::loader::Level &level)
{
    glm::vec3 spawn(0.f, 41.f, 0.f);
    for (const auto &thing : level.things)
        if (thing.type == 1)
            spawn = glm::vec3(thing.position.x, 41.f, thing.position.y);
    return spawn;
}
} // namespace

/// @brief Init Player and Camera entity components
void utils::InitGame::InitPlayer(Engine::Core &core, const game::loader::Level &level, GLFWwindow *window)
{
    auto player = core.CreateEntity();
    player.AddComponent<Object::Component::Transform>(SetSpawn(level));
    auto &camera = player.AddComponent<Object::Component::Camera>();
    camera.farPlane = 10000.f;
    camera.nearPlane = 1.f;
    double posx = 0;
    glfwGetCursorPos(window, &(posx), nullptr);
    player.AddComponent<game::component::LastMousePos>(static_cast<float>(posx));
    player.AddComponent<game::component::Sensibility>();
    player.AddComponent<game::component::Player>();
    player.AddComponent<game::component::Velocity>();
    player.AddComponent<game::component::Speed>();
}