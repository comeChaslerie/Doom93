#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace game::loader {
class WadLoadError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct LumpEntry {
    std::int32_t filePos = 0;
    std::int32_t size = 0;
    std::string name;
};
struct WadInfo {
    std::string magic;
    int32_t nb_lumps = 0;
    int32_t offset = 0;
    std::vector<LumpEntry> lumps;
};
WadInfo WadLoader(std::string filepath);
}   // namespace game::loader
