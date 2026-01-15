/// @file   AssetUtils.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  A set of static utility functions used in the Asset Manager system
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <string>

namespace LaurelEye::IO {

    inline std::string extractExtension(const std::string& path) {
        auto dot = path.find_last_of('.');
        if ( dot == std::string::npos ) return "";
        std::string ext = path.substr(dot + 1);
        // Normalize to lowercase for consistency
        for ( auto& c : ext ) c = static_cast<char>(::tolower(c));
        return ext;
    }
    inline std::string extractName(const std::string& path) {
        auto lastSlash = path.find_last_of("/\\");
        auto dot = path.find_last_of('.');
        return path.substr(lastSlash + 1, dot - lastSlash - 1);
    }

} // namespace LaurelEye::IO
