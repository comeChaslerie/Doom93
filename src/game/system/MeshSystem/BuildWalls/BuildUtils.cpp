#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"

void game::system::AddNormal(const glm::ivec2 &start, const glm::ivec2 &end, std::vector<glm::vec3> &normals)
{
    glm::vec3 dir = glm::vec3(end.x - start.x, 0.f, end.y - start.y);
    glm::vec3 normal = glm::normalize(glm::vec3(dir.z, 0.f, -dir.x));

    for (int i = 0; i < 4; i++)
        normals.push_back(normal);
}
void game::system::AddTextCoords(game::system::MeshConstructor &walls)
{
    walls.texcoord.push_back({0, 0});
    walls.texcoord.push_back({1, 0});
    walls.texcoord.push_back({1, 1});
    walls.texcoord.push_back({0, 1});
}
void game::system::AddIndices(game::system::MeshConstructor &walls)
{
    uint32_t base = walls.vertices.size();

    walls.indices.insert(walls.indices.end(), {base, base + 1, base + 2, base + 2, base + 3, base});
}
void game::system::AddVertices(game::system::MeshConstructor &wall, const glm::ivec2 &start, const glm::ivec2 &end,
                               float yBottom, float yTop)
{
    wall.vertices.push_back(glm::vec3(start.x, yBottom, start.y));
    wall.vertices.push_back(glm::vec3(end.x, yBottom, end.y));
    wall.vertices.push_back(glm::vec3(end.x, yTop, end.y));
    wall.vertices.push_back(glm::vec3(start.x, yTop, start.y));
}
void game::system::AddQuad(game::system::MeshConstructor &wall, int yBottom, int yTop, const glm::ivec2 &start,
                           const glm::ivec2 &end)
{
    AddIndices(wall);
    AddVertices(wall, start, end, static_cast<float>(yBottom), static_cast<float>(yTop));
    game::system::AddNormal(start, end, wall.normals);
    AddTextCoords(wall);
}
