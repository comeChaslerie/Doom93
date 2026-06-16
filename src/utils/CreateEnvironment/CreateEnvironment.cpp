#include "utils/CreateEnvironment/CreateEnvironment.hpp"
#include "component/Material.hpp"
#include "component/Transform.hpp"
#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "resource/Texture.hpp"
#include "entity/Entity.hpp"

namespace {
static constexpr glm::uvec2 textureSize = glm::uvec2(64, 64);
}

/// @brief Create Walls meshes
void utils::CreateEnvironment::CreateWalls(Engine::Core &core, const game::loader::Level &level)
{
    auto walls = core.CreateEntity();
    walls.AddComponent<Object::Component::Transform>();
    walls.AddComponent<Object::Component::Mesh>(game::system::BuildWalls(level));
}

/// @brief Create Floor and Ceiling meshes
void utils::CreateEnvironment::CreateFloorAndCeiling(Engine::Core &core, const auto &level,
    const auto &lumpData, const auto &deviceContext,
    const auto &queue, auto &textureContainer)
{
    for (auto &&[tex, mesh] : game::system::BuildFloorCeil(level)) {
        auto it = std::find_if(lumpData.flats.begin(), lumpData.flats.end(),
            [&tex](const game::loader::Flat &f) { return f.name == tex; });
        if (it == lumpData.flats.end())
            continue;
        auto entity = core.CreateEntity();
        entity.AddComponent<Object::Component::Transform>();
        entity.AddComponent<Object::Component::Mesh>(mesh);
        const game::loader::Flat &flat = *it;
        auto texture = Graphic::Resource::Texture(deviceContext, queue,
        tex, textureSize, [&flat](glm::uvec2 pixel) { return flat.pixels[pixel.y * 64 + pixel.x]; });
        textureContainer.Add(flat.name, std::move(texture));
        Object::Component::Material mat;
        mat.diffuseTexName = flat.name;
        entity.AddComponent<Object::Component::Material>(std::move(mat));
    }
}
