#pragma once

#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"
#include <map>

namespace game::system {
std::map<std::string, Object::Component::Mesh> BuildWalls(const game::loader::Level &level);
struct MeshConstructor {
    std::vector<glm::vec3> vertices;
    std::vector<glm::uint32_t> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoord;
};
} // namespace game::system
