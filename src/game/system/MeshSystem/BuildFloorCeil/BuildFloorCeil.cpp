#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/SubsectorPolygon.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "glm/fwd.hpp"
#include <cstdint>
#include <map>
#include <vector>

namespace {
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
void AddFloorCeil(game::system::MeshConstructor &FloorCeil, const std::vector<glm::vec2> &polygon, float height,
                  bool ceil)
{
    glm::uint32_t base = FloorCeil.vertices.size();

    for (const auto &pol : polygon)
    {
        FloorCeil.vertices.push_back(glm::vec3(pol.x, height, pol.y));
        if (ceil)
            FloorCeil.normals.push_back({0.f, -1.f, 0.f});
        else
            FloorCeil.normals.push_back({0.f, 1.f, 0.f});
        FloorCeil.texcoord.push_back({pol.x / 64.f, pol.y / 64.f});
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
    std::vector<std::vector<glm::vec2>> polygons = BuildSubsectorPolygons(level);

    for (std::size_t i = 0; i < level.subsectors.size(); i++)
    {
        if (polygons[i].empty()) // degenerate subsector, nothing to draw
            continue;
        const game::loader::Sector &sector = GetSector(level.subsectors[i], level);
        AddFloorCeil(floorCeil[sector.ceilingTexture], polygons[i], static_cast<float>(sector.ceilingHeight), true);
        AddFloorCeil(floorCeil[sector.floorTexture], polygons[i], static_cast<float>(sector.floorHeight), false);
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
