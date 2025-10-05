/// @file   ImageImporter.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  Handles the importing of image files, converting them to image data
/// to be used by some other system, like renderer.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/io/IAssetImporter.h"
#include <stb_image.h>

namespace LaurelEye::IO {
    class ImageImporter : public IAssetImporter {
    public:
        /// @brief Imports Shaders. Specifically uses stbi_load to populate an ImageAsset
        /// @param path - Path to the image file
        /// @return an ImageAsset loaded with the raw image data
        std::shared_ptr<IAsset> import(const std::string& path) override;
    };
} // namespace LaurelEye::IO
