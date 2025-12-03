/// @file   AssetManager.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  Manager of I/O related operations, as well as managing a cache
/// of I/O fetched data blobs
/// A data blob is NOT a system specific Object, but instead a series of primitives.
/// For example, a .obj could be loaded to create a MeshAsset, which is a series of float
/// arrays and unsigned ints. These data blobs can then be converted to system level Objects
/// in either their respective Systems, or an intermediary layer.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include "LaurelEyeEngine/io/IAsset.h"
#include "LaurelEyeEngine/io/IAssetImporter.h"
#include "LaurelEyeEngine/core/EngineContext.h"

namespace LaurelEye::IO {

    class AssetManager {
    public:
        AssetManager(EngineContext& ctx)
            : context(&ctx) {}

        void initialize();
        /// @brief Given an extension for a file type, and an asset importer, update our importers map
        /// @param extension The actual extension type of the data files to be imported. I.e.; ".obj"
        /// @param importer The importer class we use to import. I.e., a MeshImporter can interpret a .obj correctly.
        void registerImporter(const std::string& extension, std::shared_ptr<IAssetImporter> importer);
        /// @brief Load an asset given a path. Super generic, no holds barred.
        /// @param path The path to the file you want to load, relative to project root
        /// @return The type of asset you would need dependent on the extension and importer used.
        /// I.e., if you asked to import a .obj you would get back a MeshAsset
        std::shared_ptr<IAsset> load(const std::string& path);
        /// @brief Removes the cached IAsset from memory
        /// @param path The path used to load the asset
        void unload(const std::string& path); 
    private:
        // Context for other engine services
        EngineContext* context = nullptr;
        // Cached assets, easy lookup if they've been encountered before. Maps filepath to the actual IAsset child
        std::unordered_map<std::string, std::shared_ptr<IAsset>> assetCache;
        // Registered importers. Specific for extension type. Maps extension string to the correlating importer
        std::unordered_map<std::string, std::shared_ptr<IAssetImporter>> importers;

        // Uses string formatting to extract the extension out of a path. I.e.; ".obj"
        static std::string getExtension(const std::string& path);
        // normalizes the extension so that like extensions can be stored together
        static std::string normalizeExtension(const std::string& ext);
    };

} // namespace LaurelEye::IO
