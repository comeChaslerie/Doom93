#include "utils/InitGame/ConfigureWindow.hpp"
#include "GLFW/glfw3.h"

/// @brief Configure Window settings
void utils::InitGame::ConfigureWindow(Engine::Core &core, GLFWwindow *window)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
