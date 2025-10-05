/// @file   IAssetImporter.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  Base for asset importer. An asset importer designates a file type and provides
/// A unique way of importing it into our systems. By importing, I mean reaching out to IO
/// and populating an "Asset" (from Assets.h)
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <memory>
#include "LaurelEyeEngine/io/IAsset.h"

namespace LaurelEye::IO {
    class IAssetImporter {
    public:
        virtual ~IAssetImporter() = default;
        virtual std::shared_ptr<IAsset> import(const std::string& path) = 0;
    };
}// namespace LaurelEye::IO
