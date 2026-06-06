// WADReaderParse.cpp
// Coeurs des fonctions de parsing des lumps (separes de WADReader.cpp pour la
// lisibilite). Chaque ParseXxx se positionne via entry.filePos puis lit les
// enregistrements champ par champ (Design B : pas de reinterpret_cast de struct).

#include "game/loader/WADReader.hpp"
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <utility>

namespace game::loader {

// ============================================================================
// Helpers
// ============================================================================

// Lit un nom char[8] et retire le padding de '\0' (les noms WAD sont 0-padded).
std::string WADReader::GetName()
{
    std::string raw = GetString(_nameSize);
    size_t end = raw.find('\0');

    if (end != std::string::npos)
        raw.resize(end);
    return raw;
}

// Convertit un index de palette (0-255) en RGBA via la palette active (PLAYPAL[0]).
glm::u8vec4 WADReader::PaletteColor(std::uint8_t index) const
{
    if (_lumpData.palettes.empty())
        return {0, 0, 0, 0};
    return _lumpData.palettes.front().colors[index];
}

// ============================================================================
// A. Geometrie de niveau
// ============================================================================

void WADReader::ParseThings(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 10;

    Seek(entry.filePos);
    level.things.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        Thing thing;
        thing.position.x = GetInt16();
        thing.position.y = GetInt16();
        thing.angle = GetInt16();
        thing.type = GetInt16();
        thing.flags = GetInt16();
        level.things.push_back(thing);
    }
}

void WADReader::ParseLinedefs(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 14;

    Seek(entry.filePos);
    level.linedefs.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        Linedef line;
        line.startVertex = GetInt16();
        line.endVertex = GetInt16();
        line.flags = GetInt16();
        line.special = GetInt16();
        line.tag = GetInt16();
        line.frontSidedef = GetInt16();
        line.backSidedef = GetInt16();
        level.linedefs.push_back(line);
    }
}

void WADReader::ParseSidedefs(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 30;

    Seek(entry.filePos);
    level.sidedefs.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        Sidedef side;
        side.offset.x = GetInt16();
        side.offset.y = GetInt16();
        side.upper = GetName();
        side.lower = GetName();
        side.middle = GetName();
        side.sector = GetInt16();
        level.sidedefs.push_back(side);
    }
}

void WADReader::ParseVertexes(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 4; // (x:int16, y:int16)

    Seek(entry.filePos);
    level.vertexes.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        glm::ivec2 vertex{0, 0};
        vertex.x = GetInt16();
        vertex.y = GetInt16();
        level.vertexes.push_back(vertex);
    }
}

void WADReader::ParseSegs(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 12;

    Seek(entry.filePos);
    level.segs.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        Seg seg;
        seg.startVertex = GetInt16();
        seg.endVertex = GetInt16();
        seg.angle = GetInt16();
        seg.linedef = GetInt16();
        seg.direction = GetInt16();
        seg.offset = GetInt16();
        level.segs.push_back(seg);
    }
}

void WADReader::ParseSubSectors(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 4;

    Seek(entry.filePos);
    level.subsectors.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        SubSector sub;
        sub.segCount = GetInt16();
        sub.firstSeg = GetInt16();
        level.subsectors.push_back(sub);
    }
}

void WADReader::ParseNodes(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 28;

    Seek(entry.filePos);
    level.nodes.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        Node node;
        node.partition.x = GetInt16();
        node.partition.y = GetInt16();
        node.direction.x = GetInt16();
        node.direction.y = GetInt16();
        // Chaque bbox disque est stockee (top, bottom, left, right).
        for (BBox *box : {&node.rightBox, &node.leftBox})
        {
            int16_t top = GetInt16();
            int16_t bottom = GetInt16();
            int16_t left = GetInt16();
            int16_t right = GetInt16();
            box->min = {left, bottom};
            box->max = {right, top};
        }
        node.rightChild = static_cast<std::uint16_t>(GetInt16());
        node.leftChild = static_cast<std::uint16_t>(GetInt16());
        level.nodes.push_back(node);
    }
}

void WADReader::ParseSectors(const LumpEntry &entry, Level &level)
{
    constexpr int32_t recordSize = 26;

    Seek(entry.filePos);
    level.sectors.reserve(entry.size / recordSize);
    for (int32_t read = 0; read + recordSize <= entry.size; read += recordSize)
    {
        Sector sector;
        sector.floorHeight = GetInt16();
        sector.ceilingHeight = GetInt16();
        sector.floorTexture = GetName();
        sector.ceilingTexture = GetName();
        sector.lightLevel = GetInt16();
        sector.special = GetInt16();
        sector.tag = GetInt16();
        level.sectors.push_back(sector);
    }
}

void WADReader::ParseReject(const LumpEntry &entry, Level &level)
{
    Seek(entry.filePos);
    level.reject.bits.reserve(entry.size);
    for (int32_t read = 0; read < entry.size; read++)
        level.reject.bits.push_back(GetUint8());
}

void WADReader::ParseBlockmap(const LumpEntry &entry, Level &level)
{
    Blockmap blockmap;

    Seek(entry.filePos);
    blockmap.origin.x = GetInt16();
    blockmap.origin.y = GetInt16();
    blockmap.columns = GetInt16();
    blockmap.rows = GetInt16();

    const int32_t blockCount = blockmap.columns * blockmap.rows;
    blockmap.offsets.reserve(blockCount);
    for (int32_t k = 0; k < blockCount; k++)
        blockmap.offsets.push_back(static_cast<std::uint16_t>(GetInt16()));

    // Pour chaque bloc : seek a son offset (en mots de 16 bits), saute le 0 de
    // tete, lit les indices de linedef jusqu'au terminateur 0xFFFF.
    blockmap.blocklists.resize(blockCount);
    for (int32_t k = 0; k < blockCount; k++)
    {
        Seek(entry.filePos + (static_cast<std::streamoff>(blockmap.offsets[k]) * 2));
        Skip(_int16Size); // 0 de tete
        for (int16_t value = GetInt16(); static_cast<std::uint16_t>(value) != 0xFFFF; value = GetInt16())
            blockmap.blocklists[k].push_back(value);
    }
    level.blockmap = std::move(blockmap);
}

// ============================================================================
// Agregat de niveau : consomme les sous-lumps qui suivent le marqueur ExMy/MAPxx.
// ============================================================================

void WADReader::ParseLevel(int32_t &i)
{
    Level level;
    level.name = _wadInfos.lumps[i].name;

    while (i + 1 < _wadInfos.nb_lumps)
    {
        const LumpEntry &sub = _wadInfos.lumps[i + 1];

        if (sub.name == "THINGS")
            ParseThings(sub, level);
        else if (sub.name == "LINEDEFS")
            ParseLinedefs(sub, level);
        else if (sub.name == "SIDEDEFS")
            ParseSidedefs(sub, level);
        else if (sub.name == "VERTEXES")
            ParseVertexes(sub, level);
        else if (sub.name == "SEGS")
            ParseSegs(sub, level);
        else if (sub.name == "SSECTORS")
            ParseSubSectors(sub, level);
        else if (sub.name == "NODES")
            ParseNodes(sub, level);
        else if (sub.name == "SECTORS")
            ParseSectors(sub, level);
        else if (sub.name == "REJECT")
            ParseReject(sub, level);
        else if (sub.name == "BLOCKMAP")
            ParseBlockmap(sub, level);
        else
            break; // plus de sous-lump : fin du niveau
        i++;
    }
    _lumpData.levels.push_back(std::move(level));
}

// ============================================================================
// F. Couleurs
// ============================================================================

void WADReader::ParsePalettes(const LumpEntry &entry)
{
    constexpr int32_t paletteSize = 256 * 3; // 256 couleurs RGB

    Seek(entry.filePos);
    for (int32_t read = 0; read + paletteSize <= entry.size; read += paletteSize)
    {
        Palette palette;
        for (glm::u8vec4 &color : palette.colors)
        {
            color.r = GetUint8();
            color.g = GetUint8();
            color.b = GetUint8();
            color.a = 255;
        }
        _lumpData.palettes.push_back(palette);
    }
}

void WADReader::ParseColormaps(const LumpEntry &entry)
{
    constexpr int32_t mapSize = 256;

    Seek(entry.filePos);
    for (int32_t read = 0; read + mapSize <= entry.size; read += mapSize)
    {
        Colormap colormap;
        for (std::uint8_t &cell : colormap.map)
            cell = GetUint8();
        _lumpData.colormaps.push_back(colormap);
    }
}

// ============================================================================
// D. Definitions de textures murales
// ============================================================================

void WADReader::ParsePnames(const LumpEntry &entry)
{
    Seek(entry.filePos);
    const int32_t count = GetInt32();
    _lumpData.pnames.reserve(count);
    for (int32_t k = 0; k < count; k++)
        _lumpData.pnames.push_back(GetName());
}

void WADReader::ParseTextures(const LumpEntry &entry)
{
    Seek(entry.filePos);
    const int32_t count = GetInt32();

    std::vector<int32_t> offsets;
    offsets.reserve(count);
    for (int32_t k = 0; k < count; k++)
        offsets.push_back(GetInt32());

    for (int32_t k = 0; k < count; k++)
    {
        Seek(entry.filePos + offsets[k]);
        TextureDef texture;
        texture.name = GetName();
        Skip(_int32Size); // masked (booleen, inutilise)
        texture.width = GetInt16();
        texture.height = GetInt16();
        Skip(_int32Size); // columndirectory (obsolete)
        const int16_t patchCount = GetInt16();
        texture.patches.reserve(patchCount);
        for (int16_t p = 0; p < patchCount; p++)
        {
            TexturePatch patch;
            patch.origin.x = GetInt16();
            patch.origin.y = GetInt16();
            patch.patchIndex = GetInt16();
            patch.stepDir = GetInt16();
            patch.colormap = GetInt16();
            texture.patches.push_back(patch);
        }
        _lumpData.textures.push_back(std::move(texture));
    }
}

// ============================================================================
// E. Graphismes
// ============================================================================

void WADReader::ParseFlat(const LumpEntry &entry)
{
    Flat flat;
    flat.name = entry.name;

    Seek(entry.filePos);
    for (glm::u8vec4 &pixel : flat.pixels)
        pixel = PaletteColor(GetUint8());
    _lumpData.flats.push_back(flat);
}

void WADReader::ParsePicture(const LumpEntry &entry)
{
    constexpr std::uint8_t columnEnd = 255;

    Seek(entry.filePos);
    Picture picture;
    picture.name = entry.name;
    picture.width = GetInt16();
    picture.height = GetInt16();
    picture.offset.x = GetInt16();
    picture.offset.y = GetInt16();

    // Offsets (depuis le debut du lump) de chaque colonne.
    std::vector<int32_t> columnOffsets;
    columnOffsets.reserve(picture.width);
    for (int32_t c = 0; c < picture.width; c++)
        columnOffsets.push_back(GetInt32());

    // Pixels transparents par defaut (alpha = 0).
    picture.pixels.assign(static_cast<size_t>(picture.width) * picture.height, glm::u8vec4{0, 0, 0, 0});

    // Format "post" : chaque colonne est une suite de spans verticaux.
    for (int32_t c = 0; c < picture.width; c++)
    {
        Seek(entry.filePos + columnOffsets[c]);
        for (std::uint8_t topDelta = GetUint8(); topDelta != columnEnd; topDelta = GetUint8())
        {
            const std::uint8_t length = GetUint8();
            Skip(1); // octet de padding (avant les pixels)
            for (std::uint8_t row = 0; row < length; row++)
            {
                const std::uint8_t index = GetUint8();
                const int32_t y = topDelta + row;
                if (y < picture.height)
                    picture.pixels[(static_cast<size_t>(y) * picture.width) + c] = PaletteColor(index);
            }
            Skip(1); // octet de padding (apres les pixels)
        }
    }
    _lumpData.pictures.push_back(std::move(picture));
}

} // namespace game::loader
