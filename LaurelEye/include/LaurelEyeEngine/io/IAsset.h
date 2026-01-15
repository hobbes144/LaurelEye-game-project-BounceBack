/// @file   IAsset.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  Base "Asset" type. I.e., something we can load off of disc. Supports basic
/// I/O related functionality, but mainly should contain just data.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <string>
#include "LaurelEyeEngine/io/AssetUtils.h"

namespace LaurelEye::IO {

    class IAsset {
    public:
        IAsset(const std::string& newPath)
            : path(newPath), name(extractName(newPath)), extension(extractExtension(newPath)) {}

        virtual ~IAsset() = default;

        // Path to the original file (for debugging / reloads)
        virtual const std::string& getPath() const { return path; }
        virtual const std::string& getName() const { return name; }
        virtual const std::string& getExtension() const { return extension; }

        // TODO - determine if this is needed
        //virtual void serialize() {}
        //virtual void deserialize() {}
    protected:
        std::string path;
        std::string name;
        std::string extension;
    };
} // namespace LaurelEye::IO
