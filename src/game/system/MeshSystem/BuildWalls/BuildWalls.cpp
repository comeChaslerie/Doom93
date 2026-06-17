#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"
#include "glm/fwd.hpp"
#include <vector>

namespace {
void AddBackFaces(game::system::BuildHelpers &helper, const game::loader::Sector &back, const game::loader::Level &level, const game::loader::Linedef &linedef)
{
    auto start = helper.end;
    auto end = helper.start;

    if (helper.front.floorHeight > back.floorHeight && level.sidedefs[linedef.backSidedef].lower != "-")
        game::system::AddQuad(helper.walls[level.sidedefs[linedef.backSidedef].lower], back.floorHeight, helper.front.floorHeight,
                              start, end);
    if (back.ceilingHeight < helper.front.ceilingHeight && helper.frontSidedef.upper != "-")
        game::system::AddQuad(helper.walls[helper.frontSidedef.upper], back.ceilingHeight, helper.front.ceilingHeight,
                              start, end);
    if (helper.front.ceilingHeight < back.ceilingHeight && level.sidedefs[linedef.backSidedef].upper != "-")
        game::system::AddQuad(helper.walls[level.sidedefs[linedef.backSidedef].upper], helper.front.ceilingHeight, back.ceilingHeight,
                              start, end);
}
void AddBackSidedef(const game::loader::Level &level, const game::loader::Linedef &linedef,
                    game::system::BuildHelpers &helper)
{
    const game::loader::Sector &back = level.sectors[level.sidedefs[linedef.backSidedef].sector];

    if (back.floorHeight > helper.front.floorHeight && helper.frontSidedef.lower != "-")
        game::system::AddQuad(helper.walls[helper.frontSidedef.lower], helper.front.floorHeight, back.floorHeight,
                              helper.start, helper.end);
    AddBackFaces(helper, back, level, linedef);
}
void AddNewWall(const game::loader::Level &level, const game::loader::Linedef &linedef,
                game::system::BuildHelpers &helper)
{
    if (linedef.backSidedef < 0)
    {
        if (helper.frontSidedef.middle == "-")
            return;
        AddQuad(helper.walls[helper.frontSidedef.middle], helper.front.floorHeight, helper.front.ceilingHeight,
                helper.start, helper.end);
    }
    else
        AddBackSidedef(level, linedef, helper);
}
} // namespace

std::map<std::string, Object::Component::Mesh> game::system::BuildWalls(const game::loader::Level &level)
{
    std::map<std::string, Object::Component::Mesh> meshes;
    std::map<std::string, MeshConstructor> walls;

    for (const auto &linedef : level.linedefs)
    {
        BuildHelpers helper(level.sectors[level.sidedefs[linedef.frontSidedef].sector],
                            level.vertexes[linedef.startVertex], level.vertexes[linedef.endVertex],
                            level.sidedefs[linedef.frontSidedef], walls);
        AddNewWall(level, linedef, helper);
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
