/// @file   MeshImporter.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  Handles the importing of 3D models using assimp.
/// Specifically, uses assimp to read vertices and indices and converts that data
/// into an IAsset derived MeshAsset to be loaded by whatever needs it (likely renderer).
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/io/IAssetImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace LaurelEye::IO {

    class MeshImporter : public IAssetImporter {
    public:
        /// @brief Imports 3D meshes. Specifically uses assimp to populate a MeshAsset
        /// @param path - Path to the mesh file
        /// @return a MeshAsset loaded with the raw mesh data (vertices, indices, etc).
        std::shared_ptr<IAsset> import(const std::string& path) override;

    private:
        std::shared_ptr<IAsset> createAsset(const std::string& path, const aiScene* scene);
        std::shared_ptr<IAsset> createSkinnedAsset(const std::string& path, const aiScene* scene);

        void addBoneData(SkinnedMeshAsset::SkinnedVertex& vertex, int boneIndex, float boneWeight);
    };

} // namespace LaurelEye::IO
