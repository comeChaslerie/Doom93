#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"
#include "glm/geometric.hpp"
#include <map>
#include <string>
#include <vector>

namespace {
void AddNormal(const glm::ivec2 &start, const glm::ivec2 &end, std::vector<glm::vec3> &normals)
{
    glm::vec3 dir = glm::vec3(end.x - start.x, 0.f, end.y - start.y);
    glm::vec3 normal = glm::normalize(glm::vec3(dir.z, 0.f, -dir.x));

    for (int i = 0; i < 4; i++)
        normals.push_back(normal);
}
void AddTextCoords(game::system::MeshConstructor &walls)
{
    walls.texcoord.push_back({0, 0});
    walls.texcoord.push_back({1, 0});
    walls.texcoord.push_back({1, 1});
    walls.texcoord.push_back({0, 1});
}
void AddIndices(game::system::MeshConstructor &walls)
{
    uint32_t base = walls.vertices.size();

    walls.indices.insert(walls.indices.end(), {base, base + 1, base + 2, base + 2, base + 3, base});
}
// One vertical quad between two heights, facing the right side of start -> end
// (the Doom "front" side). "-" marks an absent texture in the WAD: no geometry.
void AddWallQuad(std::map<std::string, game::system::MeshConstructor> &walls, const std::string &texture,
                 const glm::ivec2 &start, const glm::ivec2 &end, float bottom, float top)
{
    if (texture.empty() || texture == "-")
        return;
    game::system::MeshConstructor &wall = walls[texture];

    AddIndices(wall);
    wall.vertices.push_back(glm::vec3(start.x, bottom, start.y));
    wall.vertices.push_back(glm::vec3(end.x, bottom, end.y));
    wall.vertices.push_back(glm::vec3(end.x, top, end.y));
    wall.vertices.push_back(glm::vec3(start.x, top, start.y));
    AddNormal(start, end, wall.normals);
    AddTextCoords(wall);
}
// A two-sided linedef joins two sectors: the floor step (lower) and the ceiling
// drop (upper) each face the sector they are visible from, so the quad is
// flipped (start/end swapped) when that sector is the back one.
void AddHalfWalls(const game::loader::Level &level, const game::loader::Linedef &linedef,
                  std::map<std::string, game::system::MeshConstructor> &walls)
{
    const game::loader::Sidedef &front = level.sidedefs[linedef.frontSidedef];
    const game::loader::Sidedef &back = level.sidedefs[linedef.backSidedef];
    const game::loader::Sector &frontSector = level.sectors[front.sector];
    const game::loader::Sector &backSector = level.sectors[back.sector];
    glm::ivec2 start = level.vertexes[linedef.startVertex];
    glm::ivec2 end = level.vertexes[linedef.endVertex];

    if (frontSector.floorHeight < backSector.floorHeight)
        AddWallQuad(walls, front.lower, start, end, static_cast<float>(frontSector.floorHeight),
                    static_cast<float>(backSector.floorHeight));
    else if (backSector.floorHeight < frontSector.floorHeight)
        AddWallQuad(walls, back.lower, end, start, static_cast<float>(backSector.floorHeight),
                    static_cast<float>(frontSector.floorHeight));
    if (frontSector.ceilingHeight > backSector.ceilingHeight)
        AddWallQuad(walls, front.upper, start, end, static_cast<float>(backSector.ceilingHeight),
                    static_cast<float>(frontSector.ceilingHeight));
    else if (backSector.ceilingHeight > frontSector.ceilingHeight)
        AddWallQuad(walls, back.upper, end, start, static_cast<float>(frontSector.ceilingHeight),
                    static_cast<float>(backSector.ceilingHeight));
}
void AddSolidWall(const game::loader::Level &level, const game::loader::Linedef &linedef,
                  std::map<std::string, game::system::MeshConstructor> &walls)
{
    const game::loader::Sidedef &front = level.sidedefs[linedef.frontSidedef];
    const game::loader::Sector &sector = level.sectors[front.sector];

    AddWallQuad(walls, front.middle, level.vertexes[linedef.startVertex], level.vertexes[linedef.endVertex],
                static_cast<float>(sector.floorHeight), static_cast<float>(sector.ceilingHeight));
}
} // namespace

std::map<std::string, Object::Component::Mesh> game::system::BuildWalls(const game::loader::Level &level)
{
    std::map<std::string, Object::Component::Mesh> meshes;
    std::map<std::string, MeshConstructor> walls;

    for (const auto &linedef : level.linedefs)
    {
        if (linedef.backSidedef < 0)
            AddSolidWall(level, linedef, walls);
        else
            AddHalfWalls(level, linedef, walls);
    }
    for (auto &[string, wall] : walls)
    {
        meshes[string].SetVertices(wall.vertices);
        meshes[string].SetIndices(wall.indices);
        meshes[string].SetNormals(wall.normals);
        meshes[string].SetTexCoords(wall.texcoord);
    }
    return meshes;
}
