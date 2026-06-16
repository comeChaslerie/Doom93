#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "glm/fwd.hpp"
#include <cstdint>
#include <map>
#include <vector>

namespace {
std::vector<glm::ivec2> GetPolygon(const game::loader::SubSector &subsector, const game::loader::Level &level)
{
    std::vector<glm::ivec2> polygon;

    for (int i = 0; i < subsector.segCount; i++)
        polygon.push_back(level.vertexes[level.segs[subsector.firstSeg + i].startVertex]);
    return polygon;
}
const game::loader::Sector &GetSector(const game::loader::SubSector &subsector, const game::loader::Level &level)
{
    int side = 0;
    const game::loader::Seg &first_seg = level.segs[subsector.firstSeg];
    const game::loader::Linedef &linedef = level.linedefs[first_seg.linedef];

    if (first_seg.direction == 0)
        side = linedef.frontSidedef;
    else
        side = linedef.backSidedef;
    return level.sectors[level.sidedefs[side].sector];
}
void AddFloorCeil(game::system::MeshConstructor &FloorCeil, std::vector<glm::ivec2> &polygon, float height, bool ceil)
{
    glm::uint32_t base = FloorCeil.vertices.size();

    for (const auto &pol : polygon)
    {
        FloorCeil.vertices.push_back(glm::vec3(pol.x, height, pol.y));
        if (ceil)
            FloorCeil.normals.push_back({0.f, -1.f, 0.f});
        else
            FloorCeil.normals.push_back({0.f, 1.f, 0.f});
        FloorCeil.texcoord.push_back(
            {static_cast<float>(pol.x) / 64.f, static_cast<float>(pol.y) / 64.f}); // au lieu de {0,0}
    }
    for (uint32_t i = 1; i + 1 < polygon.size(); i++)
    {
        if (ceil)
            FloorCeil.indices.insert(FloorCeil.indices.end(), {base, base + i, base + i + 1});
        else
            FloorCeil.indices.insert(FloorCeil.indices.end(), {base, base + i + 1, base + i});
    }
}
} // namespace

std::map<std::string, Object::Component::Mesh> game::system::BuildFloorCeil(const game::loader::Level &level)
{
    std::map<std::string, Object::Component::Mesh> meshes;
    std::map<std::string, MeshConstructor> floorCeil;
    std::vector<glm::ivec2> polygon;

    for (const auto &subsector : level.subsectors)
    {
        polygon = GetPolygon(subsector, level);
        const game::loader::Sector &sector = GetSector(subsector, level);
        AddFloorCeil(floorCeil[sector.ceilingTexture], polygon, static_cast<float>(sector.ceilingHeight), true);
        AddFloorCeil(floorCeil[sector.floorTexture], polygon, static_cast<float>(sector.floorHeight), false);
    }
    for (auto &&[tex, flat] : floorCeil)
    {
        Object::Component::Mesh mesh;
        mesh.SetIndices(floorCeil[tex].indices);
        mesh.SetVertices(floorCeil[tex].vertices);
        mesh.SetNormals(floorCeil[tex].normals);
        mesh.SetTexCoords(floorCeil[tex].texcoord);
        meshes[tex] = std::move(mesh);
    }
    return meshes;
}
