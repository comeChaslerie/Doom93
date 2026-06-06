#include "utils/FileReader/FileReader.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <system_error>
#include <vector>

using utils::FileReader::FileReader;
using utils::FileReader::FileReaderError;

namespace {
// Fixture : ecrit des octets bruts dans un fichier temporaire unique (nomme
// d'apres le test courant) et le nettoie a la fin.
class FileReaderTest : public ::testing::Test {
  protected:
    std::filesystem::path _path;

    std::string WriteBytes(const std::vector<std::uint8_t> &bytes)
    {
        _path = std::filesystem::temp_directory_path() /
                (std::string("filereader_") + ::testing::UnitTest::GetInstance()->current_test_info()->name() + ".bin");
        std::ofstream out(_path, std::ios::binary);
        out.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        return _path.string();
    }

    void TearDown() override
    {
        std::error_code ec;
        std::filesystem::remove(_path, ec);
    }
};
} // namespace

TEST_F(FileReaderTest, GetStringReadsExactBytes)
{
    FileReader reader(WriteBytes({'D', 'O', 'O', 'M'}));

    EXPECT_EQ(reader.GetString(4), "DOOM");
}

TEST_F(FileReaderTest, GetInt32IsLittleEndian)
{
    FileReader reader(WriteBytes({0x04, 0x03, 0x02, 0x01}));

    EXPECT_EQ(reader.GetInt32(), 0x01020304);
}

TEST_F(FileReaderTest, GetInt16IsLittleEndian)
{
    FileReader reader(WriteBytes({0x34, 0x12}));

    EXPECT_EQ(reader.GetInt16(), 0x1234);
}

TEST_F(FileReaderTest, GetUint8ReadsOneByte)
{
    FileReader reader(WriteBytes({0xAB}));

    EXPECT_EQ(reader.GetUint8(), 0xAB);
}

TEST_F(FileReaderTest, SeekJumpsToAbsolutePosition)
{
    FileReader reader(WriteBytes({0xDE, 0xAD, 'H', 'i'}));

    reader.Seek(2);
    EXPECT_EQ(reader.GetString(2), "Hi");
}

TEST_F(FileReaderTest, SkipAdvancesFromCurrentPosition)
{
    FileReader reader(WriteBytes({'A', 'B', 'C', 'D'}));

    EXPECT_EQ(reader.GetString(1), "A");
    reader.Skip(2);
    EXPECT_EQ(reader.GetString(1), "D");
}

TEST_F(FileReaderTest, CheckFileIntegrityPassesWhenEnoughBytesRead)
{
    FileReader reader(WriteBytes({0x01, 0x00, 0x00, 0x00}));

    EXPECT_EQ(reader.GetInt32(), 1);
    EXPECT_NO_THROW(reader.CheckFileIntegrity(FileReader::_int32Size, FileReader::CheckState::BytesOnly));
}

TEST_F(FileReaderTest, CheckFileIntegrityThrowsOnByteCountMismatch)
{
    FileReader reader(WriteBytes({0x01, 0x02})); // 2 octets pour une lecture de 4

    static_cast<void>(reader.GetInt32());
    EXPECT_THROW(reader.CheckFileIntegrity(FileReader::_int32Size, FileReader::CheckState::BytesOnly), FileReaderError);
}

TEST_F(FileReaderTest, CheckFileIntegrityThrowsOnTruncatedStream)
{
    FileReader reader(WriteBytes({0x01})); // 1 octet pour une lecture de 2

    static_cast<void>(reader.GetInt16());
    EXPECT_THROW(reader.CheckFileIntegrity(0, FileReader::CheckState::StreamOnly), FileReaderError);
}
