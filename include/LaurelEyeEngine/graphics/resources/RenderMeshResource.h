#pragma once

#include "LaurelEyeEngine/graphics/resources/GpuMesh.h"
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
#include "LaurelEyeEngine/math/Matrix4.h"

#include <cstdint>
#include <string>

namespace LaurelEye::Graphics {

    class RenderMeshResource {
    public:
        MeshHandle handle = RenderMesh::InvalidMesh;
        std::string name;

        GpuMesh gpu;
        SkeletonHandle skeleton = Skeleton::InvalidSkeleton;

        // Local-space bounds from vertex positions
        // AABB        localBounds{};

        uint32_t maxBonesPerVertex{0};
        // One inverse bind matrix per bone in this mesh's rig
        std::vector<Matrix4> inverseBindMatrices;

        bool isSkinned() const noexcept {
            return isValidSkeleton(skeleton) &&
                   maxBonesPerVertex > 0;
        }

        bool alive = false;
    };
} // namespace LaurelEye::Graphics
