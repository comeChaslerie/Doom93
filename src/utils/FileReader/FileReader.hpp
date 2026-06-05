#pragma once

#include <stdexcept>

namespace utils::FileReader {
class FileReaderError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
void checkFileIntegrity(std::ifstream &file, size_t nbBytes, bool checkEnd);
[[nodiscard]] int32_t GetInt32(std::ifstream &file);
[[nodiscard]] std::string GetString(std::ifstream &file, size_t size);
constexpr size_t int32Size = 4;
} // namespace utils::FileReader
