#include "LaurelEyeEngine/io/FileSystem.h"
#include <iostream>

namespace LaurelEye::IO {
    namespace fs = std::filesystem;

    fs::path resolve(const std::string& relativePath) {
#ifdef PROJECT_ROOT_DIR
        std::cout << PROJECT_ROOT_DIR << std::endl;
        return fs::path(PROJECT_ROOT_DIR) / relativePath;
#else
        throw std::runtime_error("PROJECT_ROOT_DIR not defined. Did you forget to set it in CMake?");
#endif
    }
} // namespace LaurelEye::IO::FileSystem
