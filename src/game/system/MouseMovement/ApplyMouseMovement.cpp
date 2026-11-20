#include "game/system/MouseMovement/ApplyMouseMovement.hpp"
#include "GLFW/glfw3.h"
#include "component/CharacterControllerInternal.hpp"
#include "component/Transform.hpp"
#include "game/component/MouseMovement/LastMousePos.hpp"
#include "game/component/MouseMovement/Sensibility.hpp"
#include "game/component/Tags/Player.hpp"
#include "game/system/MouseMovement/ComputeMouseMovement.hpp"
#include "glm/fwd.hpp"
#include "resource/Window.hpp"
#include "utils/JoltConversions.hpp"
#include <glm/gtc/quaternion.hpp>

void game::system::ApplyMouseMovement(Engine::Core &core)
{
    auto window = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
    auto &registry = core.GetRegistry();
    for (auto &&[entity, transform, lastMousePos, sensibiliy] :
         registry
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

        // The physics plugin rewrites the Transform rotation from its character every fixed tick, so the
        // character has to turn as well or the camera snaps back.
        auto *character = registry.try_get<Physics::Component::CharacterControllerInternal>(entity);
        if (character != nullptr && character->IsValid())
            character->character->SetRotation(Physics::Utils::ToJoltQuat(rotation));
    }
}
