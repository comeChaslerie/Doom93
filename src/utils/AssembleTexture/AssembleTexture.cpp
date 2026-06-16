#include "utils/AssembleTexture/AssembleTexture.hpp"
#include "game/loader/LumpsData.hpp"
#include "glm/ext/vector_int2.hpp"
#include "glm/ext/vector_uint4_sized.hpp"
#include <cstddef>
#include <string>

namespace {
void SetTextureLine(utils::AssembleTexture::AssembledTexture &texture, const game::loader::TextureDef &textureDef,
                    auto &picture, int p_y, auto &patch)
{
    for (int p_x = 0; p_x < picture->width; p_x++)
    {
        glm::ivec2 canva(patch.origin.x + p_x, patch.origin.y + p_y);
        if (canva.x < 0 || canva.x >= textureDef.width || canva.y < 0 || canva.y >= textureDef.height)
            continue;
        glm::u8vec4 src = picture->pixels[p_y * picture->width + p_x];
        if (src.a > 0)
            texture.pixels[canva.y * textureDef.width + canva.x] = src;
    }
}
void SetTexture(const game::loader::TextureDef &textureDef, const std::vector<game::loader::Picture> &pictures,
                const std::string &patchName, utils::AssembleTexture::AssembledTexture &texture, auto &patch)
{
    auto picture =
        std::ranges::find_if(pictures.begin(), pictures.end(), [&patchName](const game::loader::Picture &pictures) {
            return pictures.name == patchName;
        });
    if (picture == pictures.end())
        return;
    for (int p_y = 0; p_y < picture->height; p_y++)
        SetTextureLine(texture, textureDef, picture, p_y, patch);
}
} // namespace

utils::AssembleTexture::AssembledTexture
utils::AssembleTexture::AssembleTexture(const game::loader::TextureDef &textureDef, const game::loader::Pnames &pnames,
                                        const std::vector<game::loader::Picture> &pictures)
{
    AssembledTexture texture;

    texture.width = textureDef.width;
    texture.height = textureDef.height;
    texture.pixels.assign(static_cast<size_t>(texture.width) * static_cast<size_t>(texture.height),
                          glm::u8vec4(0, 0, 0, 0));
    for (auto &patch : textureDef.patches)
    {
        const std::string &patchName = pnames.at(patch.patchIndex);
        SetTexture(textureDef, pictures, patchName, texture, patch);
    }
    return texture;
}
