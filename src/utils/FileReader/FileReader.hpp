#pragma once

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>

namespace utils::FileReader {
class FileReaderError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
class FileReader {
  public:
    enum class CheckState {
        BytesOnly,
        StreamOnly,
        BytesAndStream
    };
    explicit FileReader(const std::string &filepath) : _file(filepath, std::ios::binary) {}
    void CheckFileIntegrity(size_t nbBytes, CheckState checkState);
    void Seek(std::streamoff pos);
    void Skip(std::streamoff nbBytes);
    [[nodiscard]] int32_t GetInt32();
    [[nodiscard]] int16_t GetInt16();
    [[nodiscard]] uint8_t GetUint8();
    [[nodiscard]] std::string GetString(size_t size);
    static constexpr size_t _int32Size = 4;
    static constexpr size_t _int16Size = 2;

  private:
    std::ifstream _file;
};
} // namespace utils::FileReader
