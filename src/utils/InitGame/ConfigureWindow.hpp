#pragma once

#include "GLFW/glfw3.h"
#include "core/Core.hpp"

namespace utils::InitGame {
void ConfigureWindow(Engine::Core &core, GLFWwindow *window);
}
