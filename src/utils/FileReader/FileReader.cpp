#include "utils/FileReader/FileReader.hpp"
#include <cstddef>
#include <fstream>

void utils::FileReader::checkFileIntegrity(std::ifstream &file, size_t nbBytes, bool checkEnd)
{
    if ((checkEnd && !file) || (nbBytes && file.gcount() != nbBytes))
        throw utils::FileReader::FileReaderError("Truncated file");
}

std::string utils::FileReader::GetString(std::ifstream &file, size_t size)
{
    std::vector<char> buf(size);

    file.read(buf.data(), static_cast<std::streamsize>(size));
    return std::string(buf.data(), size);
}

int32_t utils::FileReader::GetInt32(std::ifstream &file)
{
    int32_t int_value = 0;

    file.read(reinterpret_cast<char *>(&int_value), utils::FileReader::int32Size);
    return int_value;
}
