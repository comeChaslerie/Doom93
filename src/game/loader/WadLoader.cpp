#include "game/loader/WadLoader.hpp"
#include "game/loader/WADReader.hpp"
#include <cstddef>
#include <cstdlib>
#include <string>

game::loader::WadInfo game::loader::WadLoader(std::string &filepath)
{
    WADReader wadReader(filepath);

    wadReader.CheckFileIntegrity(0, game::loader::WADReader::CheckState::StreamOnly);
    wadReader.GetHeader();
    wadReader.GetWADDirectory();
    return wadReader.GetWadInfos();
}
