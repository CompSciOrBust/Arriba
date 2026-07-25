#pragma once

#include <string>

namespace Arriba::FS {

inline std::string resolveAssetPath(const std::string& path) {
    #ifdef __SWITCH__
    return "romfs:/" + path;
    #else
    return path;
    #endif
}

}  // namespace Arriba::FS
