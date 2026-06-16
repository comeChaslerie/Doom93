#include "utils/CreateEnvironment/CreateEnvironment.hpp"
#include "component/Material.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "glm/ext/vector_uint2.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/Queue.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/AssembleTexture/AssembleTexture.hpp"
#include <algorithm>

namespace {
static constexpr glm::uvec2 textureSize = glm::uvec2(64, 64);
}

/// @brief Create Walls meshes
void utils::CreateEnvironment::CreateWalls(Engine::Core &core, const game::loader::Level &level,
                                           const game::loader::LumpData &lumpData,
                                           const Graphic::Resource::DeviceContext &deviceContext,
                                           const Graphic::Resource::Queue &queue,
                                           Graphic::Resource::TextureContainer &textureContainer)
{
    for (auto &&[tex, mesh] : game::system::BuildWalls(level))
    {
        auto wall = std::ranges::find_if(lumpData.textures.begin(), lumpData.textures.end(),
                                 [&tex](const game::loader::TextureDef &def) { return def.name == tex; });
        if (wall == lumpData.textures.end())
            continue;
        auto assembled = AssembleTexture::AssembleTexture(*wall, lumpData.pnames, lumpData.pictures);
        auto walls = core.CreateEntity();
        walls.AddComponent<Object::Component::Transform>();
        walls.AddComponent<Object::Component::Mesh>(mesh);
        glm::uvec2 size(assembled.width, assembled.height);
        auto texture = Graphic::Resource::Texture(deviceContext, queue, tex, size, [&assembled](glm::uvec2 p) {
            return assembled.pixels[p.y * assembled.width + p.x];
        });
        textureContainer.Add(tex, std::move(texture));
        Object::Component::Material mat;
        mat.diffuseTexName = wall->name;
        walls.AddComponent<Object::Component::Material>(std::move(mat));
    }
}

/// @brief Create Floor and Ceiling meshes
void utils::CreateEnvironment::CreateFloorAndCeiling(Engine::Core &core, const game::loader::Level &level,
                                                     const game::loader::LumpData &lumpData,
                                                     const Graphic::Resource::DeviceContext &deviceContext,
                                                     const Graphic::Resource::Queue &queue,
                                                     Graphic::Resource::TextureContainer &textureContainer)
{
    for (auto &&[tex, mesh] : game::system::BuildFloorCeil(level))
    {
        auto flat = std::ranges::find_if(lumpData.flats.begin(), lumpData.flats.end(),
                                 [&tex](const game::loader::Flat &f) { return f.name == tex; });
        if (flat == lumpData.flats.end())
            continue;
        auto entity = core.CreateEntity();
        entity.AddComponent<Object::Component::Transform>();
        entity.AddComponent<Object::Component::Mesh>(mesh);
        auto texture = Graphic::Resource::Texture(deviceContext, queue, tex, textureSize, [&flat](glm::uvec2 pixel) {
            return flat->pixels[pixel.y * 64 + pixel.x];
        });
        textureContainer.Add(flat->name, std::move(texture));
        Object::Component::Material mat;
        mat.diffuseTexName = flat->name;
        entity.AddComponent<Object::Component::Material>(std::move(mat));
    }
}
