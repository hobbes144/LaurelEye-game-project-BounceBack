#pragma once

#include <cstdint>

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/GpuMesh.h"
#include "LaurelEyeEngine/graphics/resources/Skeleton.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye::Graphics {

    enum class PrimitiveMeshType {
        Square,
        Cube,
        Sphere,
        Invalid
    };

    /// Skinned mesh vertex
    ///
    /// - boneIndices: 4 bone indices packed as uint8_t
    ///                (0..255, you can extend later if needed)
    /// - boneWeights: 4 weights (usually normalized such that sum ≈ 1)
    struct SkinnedMeshVertex {
        Vector3 position;
        Vector3 normal;
        Vector2 uv;
        Vector3 tangent;
        IVector4 boneIndices;
        Vector4 boneWeights;
    };

    /// Static mesh vertex
    struct MeshVertex {
        Vector3 position;
        Vector3 normal;
        Vector2 uv;
        Vector3 tangent;

        MeshVertex() = default;
        MeshVertex(const SkinnedMeshVertex& vert)
            : position(vert.position), normal(vert.normal),
              uv(vert.uv), tangent(vert.tangent) {}
    };

    using MeshHandle = uint32_t;

    class RenderMesh {
    public:
        static constexpr MeshHandle InvalidMesh = UINT32_MAX;

        MeshHandle handle = RenderMesh::InvalidMesh;
        std::string name;

        GpuMesh gpu;
        SkeletonHandle skeleton = Skeleton::InvalidSkeleton;

        // Local-space bounds from vertex positions
        // AABB        localBounds{};

        uint32_t maxBonesPerVertex{0};
        // One inverse bind matrix per bone in this mesh's rig
        std::vector<Matrix4> inverseBindMatrices;
        DataBufferHandle skinDataBuffer = DataBuffer::InvalidDataBuffer;

        bool isSkinned() const noexcept {
            return isValidSkeleton(skeleton) &&
                   maxBonesPerVertex > 0;
        }

        bool alive = false;
    };

    inline bool isValidMesh(MeshHandle h) noexcept {
        return h != RenderMesh::InvalidMesh;
    }

} // namespace LaurelEye::Graphics
