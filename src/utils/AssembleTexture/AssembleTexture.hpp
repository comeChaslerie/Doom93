#pragma once

#include "game/loader/LumpsData.hpp"
#include "glm/fwd.hpp"
#include <cstdint>
#include <vector>

namespace utils::AssembleTexture {
struct AssembledTexture {
    std::int32_t width;
    std::int32_t height;
    std::vector<glm::u8vec4> pixels;
};
AssembledTexture AssembleTexture(const game::loader::TextureDef &textureDef, const game::loader::Pnames &pnames,
                                 const std::vector<game::loader::Picture> &pictures);
} // namespace utils::AssembleTexture
