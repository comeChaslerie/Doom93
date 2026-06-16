#pragma once

#include "core/Core.hpp"
#include "game/loader/LumpsData.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/Queue.hpp"
#include "resource/TextureContainer.hpp"

namespace utils::CreateEnvironment {
void CreateWalls(Engine::Core &core, const game::loader::Level &level, const game::loader::LumpData &lumpData,
                 const Graphic::Resource::DeviceContext &deviceContext, const Graphic::Resource::Queue &queue,
                 Graphic::Resource::TextureContainer &textureContainer);
void CreateFloorAndCeiling(Engine::Core &core, const game::loader::Level &level, const game::loader::LumpData &lumpData,
                           const Graphic::Resource::DeviceContext &deviceContext, const Graphic::Resource::Queue &queue,
                           Graphic::Resource::TextureContainer &textureContainer);
} // namespace utils::CreateEnvironment
