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
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/io/IAssetImporter.h"
#include "LaurelEyeEngine/math/Transform.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cstdint>

namespace LaurelEye::IO {

    /// @class MeshImporter
    /// @brief Importer for all Mesh and related resources.
    ///
    /// This includes: Mesh, Skin, Skeleton.
    ///
    /// Temporarily also includes Animations.
    ///
    /// Note regarding use and cleanup:
    /// The Mesh Asset should only need to be loaded into the RenderSystem
    /// once. The repeated uses will reuse the name of the resources. This
    /// means we can cleanup the asset so the raw data can be deleted after
    /// first load into system. This is in place for a future scene asset
    /// manager that loads resources into the systems instead of relying on
    /// component register.
    ///
    /// For now, after load into system, components will remove the pointers
    /// to the assets, since they don't need them anymore.
    ///
    /// Note regarding future of this Importer:
    /// Ideally we should change this to a general AssimpImporter maybe,
    /// because Assimp can load a lot more than just models and animations.
    ///
    /// This AssimpImporter would independently read the fbx or other Assimp
    /// supported file for all the asset names, such as meshes, materials,
    /// skeletons, assets, and Node Graph. Then it only loads in the relevant
    /// data based on what the Scene json file asks for from this file.
    ///
    /// We could have a flag saying use the file as the scene, in which case we
    /// just create all the objects in the scene from the file along with the
    /// tree from it. If we need only specific assets, we can send a list for
    /// each resource type like a list of animations, meshes and so on that
    /// should specifically be loaded.
    ///
    class MeshImporter : public IAssetImporter {
    public:
        /// @brief Imports 3D meshes. Specifically uses assimp to populate a MeshAsset
        /// @param path - Path to the mesh file
        /// @return a MeshAsset loaded with the raw mesh data (vertices, indices, etc).
        std::shared_ptr<IAsset> import(const std::string& path) override;

    private:
        std::shared_ptr<IAsset> createAsset(const std::string& path, const aiScene* scene);
        std::shared_ptr<IAsset> createSkinnedAsset(const std::string& path, const aiScene* scene);

        void buildSkeletonRecursive(const aiNode* node,
                                    int parentIndex,
                                    SkeletonAsset& skel,
                                    Transform accumulatedBindTransform,
                                    std::unordered_set<std::string>& usedBoneNames);
        std::shared_ptr<SkeletonAsset> buildSkeleton(const std::string& path, const aiScene* scene);
        std::shared_ptr<AnimationAsset> buildAnimation(const std::string& path, const aiScene* scene, const SkeletonAsset& skeleton);
        void addBoneData(SkinnedMeshAsset::SkinnedVertex& vertex, int boneIndex, float boneWeight);

        template <std::convertible_to<LaurelEye::Graphics::MeshVertex> TVertex>
        void populateVerticesIndices(std::vector<TVertex>& vertices,
                                     std::vector<uint32_t>& indices,
                                     const aiMesh* mesh,
                                     uint32_t vertexOffset = 0);
    };

} // namespace LaurelEye::IO
