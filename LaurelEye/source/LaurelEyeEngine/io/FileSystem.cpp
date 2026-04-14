#include "LaurelEyeEngine/io/FileSystem.h"

#ifndef PROJECT_ROOT_DIR
#error "MY_MACRO must be defined for this project to compile."
#endif

namespace LaurelEye::IO {
    namespace fs = std::filesystem;

    fs::path resolve(const std::string& relativePath) {
        return fs::path(PROJECT_ROOT_DIR) / relativePath;
    }
} // namespace LaurelEye::IO::FileSystem
