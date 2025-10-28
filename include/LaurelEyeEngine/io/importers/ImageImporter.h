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

#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/io/IAssetImporter.h"

namespace LaurelEye::Graphics {
    class RenderResources;
}

namespace LaurelEye::IO {
    class ImageImporter : public IAssetImporter {
    public:
        /// @brief Imports Shaders. Specifically uses stbi_load to populate an ImageAsset
        /// @param path - Path to the image file
        /// @return an ImageAsset loaded with the raw image data
        std::shared_ptr<IAsset> import(const std::string& path) override;

        void registerRenderResources(LaurelEye::Graphics::RenderResources* _rs) {
            rs = _rs;
        }

    private:
        std::shared_ptr<IAsset> loadHDRFile(const std::string& path);
        std::shared_ptr<IAsset> loadSDRFile(const std::string& path);

        LaurelEye::Graphics::RenderResources* rs = nullptr;
    };

} // namespace LaurelEye::IO
