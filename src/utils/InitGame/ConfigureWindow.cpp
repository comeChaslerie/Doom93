#include "utils/InitGame/ConfigureWindow.hpp"
#include "GLFW/glfw3.h"

/// @brief Configure Window settings
void ConfigureWindow(Engine::Core &core, auto &window) { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
