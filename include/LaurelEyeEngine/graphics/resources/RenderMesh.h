#pragma once

#include <cstdint>

#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye::Graphics {

    enum class PrimitiveMeshType {
        Square,
        Cube,
        Sphere,
        Invalid
    };

    /// Static mesh vertex
    struct MeshVertex {
        Vector3 position;
        Vector3 normal;
        Vector2 uv;
        Vector3 tangent;
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

    using MeshHandle = uint32_t;

    namespace RenderMesh {
        static constexpr MeshHandle InvalidMesh = UINT32_MAX;
    }

    inline bool isValidMesh(MeshHandle h) noexcept {
        return h != RenderMesh::InvalidMesh;
    }

    using SkeletonHandle = uint32_t;

    namespace Skeleton {
        static constexpr SkeletonHandle InvalidSkeleton = UINT32_MAX;
    }

    inline bool isValidSkeleton(SkeletonHandle h) noexcept {
        return h != Skeleton::InvalidSkeleton;
    }

} // namespace LaurelEye::Graphics
