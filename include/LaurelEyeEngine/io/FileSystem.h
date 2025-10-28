#pragma once
#include <filesystem>
#include <string>

namespace LaurelEye::IO {
    namespace fs = std::filesystem;

    /// @brief Combines a relative path (like "assets/sceneList.json") with project root
    fs::path resolve(const std::string& relativePath);
} // namespace LaurelEye::IO
