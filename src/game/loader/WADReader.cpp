#include "game/loader/WADReader.hpp"
#include "utils/FileReader/FileReader.hpp"
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <string>

namespace {
// Contexte de lecture courant pour les lumps delimites par marqueurs.
enum class Section {
    None,
    Flats,
    Sprites
};
} // namespace

void game::loader::WADReader::GetHeader()
{
    _wadInfos.magic = GetString(_magicSize);
    CheckFileIntegrity(_magicSize, FileReader::CheckState::BytesAndStream);
    if (_wadInfos.magic != "IWAD" && _wadInfos.magic != "PWAD")
        throw game::loader::WadLoadError("bad WAD magic " + _wadInfos.magic);
    _wadInfos.nb_lumps = GetInt32();
    CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
    _wadInfos.offset = GetInt32();
    CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
}

void game::loader::WADReader::GetWADDirectory()
{
    game::loader::LumpEntry lump;

    if (_wadInfos.offset < 0 || _wadInfos.nb_lumps < _headerEnd)
        throw game::loader::WadLoadError("bad header infos");
    Seek(_wadInfos.offset);
    CheckFileIntegrity(0, FileReader::CheckState::StreamOnly);
    _wadInfos.lumps.reserve(_wadInfos.nb_lumps);
    for (int32_t index = 0; index < _wadInfos.nb_lumps; index++)
    {
        CheckFileIntegrity(0, FileReader::CheckState::StreamOnly);
        lump.filePos = GetInt32();
        CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
        lump.size = GetInt32();
        CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
        lump.name = GetName();
        CheckFileIntegrity(_nameSize, FileReader::CheckState::BytesOnly);
        _wadInfos.lumps.push_back(lump);
    }
}

game::loader::LumpEntry game::loader::WADReader::GetLump(int32_t index) { return _wadInfos.lumps[index]; }

bool game::loader::WADReader::IsLevelMarker(const std::string &name)
{
    constexpr size_t exmySize = 4; // "ExMy"
    constexpr size_t mapSize = 5;  // "MAPxx"

    if (name.size() == exmySize && name[0] == 'E' && (std::isdigit(name[1]) != 0) && name[2] == 'M' &&
        (std::isdigit(name[3]) != 0))
        return true;
    if (name.size() == mapSize && name.compare(0, 3, "MAP") == 0 && (std::isdigit(name[3]) != 0) &&
        (std::isdigit(name[4]) != 0))
        return true;
    return false;
}

void game::loader::WADReader::GetLumpsData()
{
    Section section = Section::None;

    for (int32_t i = 0; i < _wadInfos.nb_lumps; i++)
    {
        const LumpEntry &entry = _wadInfos.lumps[i];
        const std::string &name = entry.name;

        if (IsLevelMarker(name))
        {
            ParseLevel(i); // consomme les sous-lumps, avance i
            continue;
        }
        // Marqueurs de section : ils basculent le contexte de lecture.
        if (name == "F_START" || name == "FF_START")
            section = Section::Flats;
        else if (name == "S_START" || name == "SS_START" || name == "P_START" || name == "PP_START")
            section = Section::Sprites;
        else if (name == "F_END" || name == "FF_END" || name == "S_END" || name == "SS_END" || name == "P_END" ||
                 name == "PP_END")
            section = Section::None;
        else if (name == "PLAYPAL")
            ParsePalettes(entry);
        else if (name == "COLORMAP")
            ParseColormaps(entry);
        else if (name == "PNAMES")
            ParsePnames(entry);
        else if (name == "TEXTURE1" || name == "TEXTURE2")
            ParseTextures(entry);
        else if (entry.size != 0)
        {
            // Lump de contenu : son type depend de la section courante.
            switch (section)
            {
            case Section::Flats: ParseFlat(entry); break;
            case Section::Sprites: ParsePicture(entry); break;
            case Section::None: break; // lump non reconnu hors section : ignore
            }
        }
    }
}

game::loader::LumpData game::loader::WADReader::GetLumpData() const { return _lumpData; }
