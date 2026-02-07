/// @file   MeshManager.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-15-2025
/// @brief Mesh Manager class that manages created meshes for the GPU.
///
/// Note: this is currently a stub. Actual logic is in the Mesh class and will
/// be moved here in the future.
#pragma once

#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
// #include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/math/Transform.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace LaurelEye::Graphics {

    class RenderResources;
    class SkeletonManager;

    class MeshManager {
    public:
        explicit MeshManager(RenderResources& _renderResources, SkeletonManager& _skeletonManager);
        ~MeshManager();

        // Mesh* createMesh(const MeshDescription& wDesc);

        MeshHandle createPrimitiveMesh(PrimitiveMeshType type);
        MeshHandle createPrimitiveMesh(const std::string& name,
                                       PrimitiveMeshType type);
        MeshHandle createSphereMesh(const std::string& name,
                                    float radius = 1.0f,
                                    uint32_t stacks = 32,
                                    uint32_t slices = 16);
        MeshHandle createSquareMesh(const std::string& name,
                                    float scale = 1.0f);
        MeshHandle createCubeMesh(const std::string& name,
                                  float scale = 1.0f);

        MeshHandle createMesh(const IO::MeshAsset* asset);
        MeshHandle createSkinnedMesh(const IO::SkinnedMeshAsset* asset,
                                     SkeletonHandle skeleton,
                                     uint32_t maxBonesPerVertex = 4);

        MeshHandle createMesh(
            const std::string& name,
            const MeshVertex* vertices,
            uint32_t vertexCount,
            const uint32_t* indices,
            uint32_t indexCount);

        // // --- Skinned mesh (skin lives in MeshAsset) ---
        MeshHandle createSkinnedMesh(
            const std::string& name,
            const SkinnedMeshVertex* vertices,
            uint32_t vertexCount,
            const uint32_t* indices,
            uint32_t indexCount,
            const Matrix4* inverseBindMatrices,
            uint32_t boneCount,
            SkeletonHandle skeleton,
            uint32_t maxBonesPerVertex = 4);

        // MeshHandle createSkinnedMesh(
        //     const std::string& name,
        //     const SkinnedMeshVertex* vertices,
        //     uint32_t vertexCount,
        //     const uint32_t* indices,
        //     uint32_t indexCount,
        //     SkeletonHandle skeletonHandle,
        //     const std::vector<uint32_t>& meshBoneIndices // mapping from mesh bones -> skeleton bones
        // );

        MeshHandle getHandle(const std::string& name) const;

        const RenderMesh* getMesh(MeshHandle h) const;
        RenderMesh* getMesh(MeshHandle h);

        void destroyMesh(MeshHandle h);

        void destroyAllMeshes();

    private:
        RenderResources& renderResources;
        SkeletonManager& skeletonManager;

        std::vector<RenderMesh> meshes;
        std::unordered_map<std::string, MeshHandle> meshNames;
        std::unordered_map<PrimitiveMeshType, MeshHandle> primitiveMeshes;

        void pushquad(std::vector<uint32_t>& indices,
                      uint32_t i, uint32_t j,
                      uint32_t k, uint32_t l,
                      uint32_t offset = 0);

        void createFace(const Transform& tr,
                        std::vector<MeshVertex>& vertices,
                        std::vector<uint32_t>& indices,
                        uint32_t indexOffset = 0);

        MeshHandle allocateMeshHandle(const std::string& name);

        // GPU creation paths:
        GpuMesh createGpuMeshForStatic(
            const std::string& meshName,
            const MeshVertex* vertices,
            uint32_t vertexCount,
            const uint32_t* indices,
            uint32_t indexCount);

        GpuMesh createGpuMeshForSkinned(
            const std::string& meshName,
            const SkinnedMeshVertex* vertices,
            uint32_t vertexCount,
            const uint32_t* indices,
            uint32_t indexCount,
            uint32_t maxBonesPerVertex);

        // Note: We don't modify the GpuMesh resource itself, but we destroy
        // the relevant GPU side resources.
        void destroyGpuMesh(const GpuMesh& r);
    };

    // Bounds:
    // This is very important for advanced rendering stuff like frustum culling and RT.
    // AABB computeLocalBounds(const MeshVertex* vertices, uint32_t vertexCount) const;
    // AABB computeLocalBounds(const SkinnedMeshVertex* vertices, uint32_t vertexCount) const;

} // namespace LaurelEye::Graphics
