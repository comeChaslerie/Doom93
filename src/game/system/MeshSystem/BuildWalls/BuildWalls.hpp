#pragma once

#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"

namespace game::system {
Object::Component::Mesh BuildWalls(const game::loader::Level &level);
struct Walls {
    std::vector<glm::vec3> vertices;
    std::vector<glm::uint32_t> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoord;
};
} // namespace game::system
