#include "game/system/MouseMovement/ApplyMouseMovement.hpp"
#include "GLFW/glfw3.h"
#include "component/Transform.hpp"
#include "game/component/MouseMovement/LastMousePos.hpp"
#include "game/component/MouseMovement/Sensibility.hpp"
#include "game/component/Tags/Player.hpp"
#include "game/system/MouseMovement/ComputeMouseMovement.hpp"
#include "glm/fwd.hpp"
#include "resource/Window.hpp"
#include <glm/gtc/quaternion.hpp>

void game::system::ApplyMouseMovement(Engine::Core &core)
{
    auto window = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
    for (auto &&[entity, transform, lastMousePos, sensibiliy] :
         core.GetRegistry()
             .view<game::component::Player, Object::Component::Transform, component::LastMousePos,
                   component::Sensibility>()
             .each())
    {
        double posx = 0;
        glfwGetCursorPos(window, &(posx), nullptr);
        float mouseDelta = (static_cast<float>(posx) - lastMousePos.lastPos) * sensibiliy.sensibility;
        lastMousePos.lastPos = static_cast<float>(posx);
        glm::quat rotation = game::system::ComputeMouseMovement(transform.GetRotation(), mouseDelta);
        transform.SetRotation(rotation);
    }
}
