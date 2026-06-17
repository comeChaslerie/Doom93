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
struct BuildHelpers {
    const game::loader::Sector &front;
    glm::ivec2 start = glm::ivec2(0, 0);
    glm::ivec2 end = glm::ivec2(0, 0);
    loader::Sidedef frontSidedef;
    std::map<std::string, MeshConstructor> &walls;
};
void AddNormal(const glm::ivec2 &start, const glm::ivec2 &end, std::vector<glm::vec3> &normals);
void AddTextCoords(game::system::MeshConstructor &walls);
void AddIndices(game::system::MeshConstructor &walls);
void AddVertices(game::system::MeshConstructor &wall, const glm::ivec2 &start, const glm::ivec2 &end, float yBottom,
                 float yTop);
void AddQuad(game::system::MeshConstructor &wall, int yBottom, int yTop, const glm::ivec2 &start,
             const glm::ivec2 &end);
} // namespace game::system
