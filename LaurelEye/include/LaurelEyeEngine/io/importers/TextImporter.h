/// @file   TextImporter.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  Handles the importing of text files. Specifically, anything containing
/// text that *isn't* JSON. So like .txt, .md, .csv, and possibly even direct scripts, like .lua
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include "LaurelEyeEngine/io/IAssetImporter.h"

namespace LaurelEye::IO {
    class TextImporter : public IAssetImporter {
    public:
        /// @brief Super simple - Uses a stringstream in order to populate a std::string field on a TextAsset
        /// @param path - Path to the text file
        /// @return an TextAsset loaded with a single std::string with the correlating text
        std::shared_ptr<IAsset> import(const std::string& path) override;
    };
} // namespace LaurelEye::IO
