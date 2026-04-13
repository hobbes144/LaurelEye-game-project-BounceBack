/// @file   JsonImporter.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  Handles the importing of JSON files. That's it.
/// Namely, will handle importing our data files, like scene and entity definitions.
/// Potentially even configs could be JSON
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include "LaurelEyeEngine/io/IAssetImporter.h"

namespace LaurelEye::IO {
    class JsonImporter : public IAssetImporter {
    public:
        /// @brief Imports JSON files. Specifically uses rapidjson::document stored in an JsonAsset
        /// @param path - Path to the JSON file
        /// @return an JsonAsset loaded with the rapidjson::document file ready for parsing
        std::shared_ptr<IAsset> import(const std::string& path) override;
    };
} // namespace LaurelEye::IO
