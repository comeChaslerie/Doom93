#include "utils/FileReader/FileReader.hpp"
#include <cstddef>
#include <fstream>
#include <ios>
#include <vector>

void utils::FileReader::FileReader::CheckFileIntegrity(size_t nbBytes, CheckState checkState)
{
    if ((checkState == CheckState::StreamOnly || checkState == CheckState::BytesAndStream) && !_file)
        throw utils::FileReader::FileReaderError("Truncated file");
    if ((checkState == CheckState::BytesOnly || checkState == CheckState::BytesAndStream) && _file.gcount() != nbBytes)
        throw utils::FileReader::FileReaderError("Truncated file");
}

std::string utils::FileReader::FileReader::GetString(size_t size)
{
    std::vector<char> buf(size);

    _file.read(buf.data(), static_cast<std::streamsize>(size));
    return std::string(buf.data(), size);
}

int32_t utils::FileReader::FileReader::GetInt32()
{
    int32_t int_value = 0;

    _file.read(reinterpret_cast<char *>(&int_value), _int32Size);
    return int_value;
}

int16_t utils::FileReader::FileReader::GetInt16()
{
    int16_t int_value = 0;

    _file.read(reinterpret_cast<char *>(&int_value), _int16Size);
    return int_value;
}

uint8_t utils::FileReader::FileReader::GetUint8()
{
    uint8_t value = 0;

    _file.read(reinterpret_cast<char *>(&value), 1);
    return value;
}

void utils::FileReader::FileReader::Seek(std::streamoff pos) { _file.seekg(pos); }
