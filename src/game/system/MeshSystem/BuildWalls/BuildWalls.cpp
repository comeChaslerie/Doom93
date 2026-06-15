#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"
#include "glm/geometric.hpp"
#include <vector>

namespace {
void AddNormal(const glm::ivec2 &start, const glm::ivec2 &end, std::vector<glm::vec3> &normals)
{
    glm::vec3 dir = glm::vec3(end.x - start.x, 0.f, end.y - start.y);
    glm::vec3 normal = glm::normalize(glm::vec3(dir.z, 0.f, -dir.x));

    for (int i = 0; i < 4; i++)
        normals.push_back(normal);
}
void AddTextCoords(game::system::Walls &walls)
{
    walls.texcoord.push_back({0, 0});
    walls.texcoord.push_back({1, 0});
    walls.texcoord.push_back({1, 1});
    walls.texcoord.push_back({0, 1});
}
void AddIndices(game::system::Walls &walls)
{
    uint32_t base = walls.vertices.size();

    walls.indices.insert(walls.indices.end(), {base, base + 1, base + 2, base + 2, base + 3, base});
}
void AddVertices(game::system::Walls &walls, const glm::ivec2 &start, const glm::ivec2 &end,
                 const game::loader::Level &level, const game::loader::Linedef &linedef)
{
    float floor = static_cast<float>(level.sectors[level.sidedefs[linedef.frontSidedef].sector].floorHeight);
    float ceil = static_cast<float>(level.sectors[level.sidedefs[linedef.frontSidedef].sector].ceilingHeight);

    walls.vertices.push_back(glm::vec3(start.x, floor, start.y));
    walls.vertices.push_back(glm::vec3(end.x, floor, end.y));
    walls.vertices.push_back(glm::vec3(end.x, ceil, end.y));
    walls.vertices.push_back(glm::vec3(start.x, ceil, start.y));
}
void AddNewWall(const game::loader::Level &level, const game::loader::Linedef &linedef, game::system::Walls &walls)
{
    glm::ivec2 start = level.vertexes[linedef.startVertex];
    glm::ivec2 end = level.vertexes[linedef.endVertex];

    AddIndices(walls);
    AddVertices(walls, start, end, level, linedef);
    AddNormal(start, end, walls.normals);
    AddTextCoords(walls);
}
} // namespace

Object::Component::Mesh game::system::BuildWalls(const game::loader::Level &level)
{
    Object::Component::Mesh mesh;
    Walls walls;

    for (const auto &linedef : level.linedefs)
        AddNewWall(level, linedef, walls);
    mesh.SetVertices(walls.vertices);
    mesh.SetIndices(walls.indices);
    mesh.SetNormals(walls.normals);
    mesh.SetTexCoords(walls.texcoord);
    return mesh;
}
