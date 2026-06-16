#pragma once

#include "core/Core.hpp"
#include "game/loader/LumpsData.hpp"

namespace utils::CreateEnvironment {
void CreateWalls(Engine::Core &core, const game::loader::Level &level);
void CreateFloorAndCeiling(Engine::Core &core, const auto &level, const auto &lumpData, const auto &deviceContext,
                           const auto &queue, auto &textureContainer);
} // namespace utils::CreateEnvironment
