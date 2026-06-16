#pragma once

#include "GLFW/glfw3.h"
#include "core/Core.hpp"
#include "game/loader/LumpsData.hpp"

namespace utils::InitGame {
void InitPlayer(Engine::Core &core, const game::loader::Level &level, GLFWwindow *window);
}
