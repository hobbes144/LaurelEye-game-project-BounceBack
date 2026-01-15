/// @file   SkeletonManager.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-29-2025
/// @brief Skeleton Manger for Skinned Meshes

#pragma once

#include "LaurelEyeEngine/graphics/resources/Skeleton.h"
#include "LaurelEyeEngine/io/Assets.h"

#include <unordered_map>
#include <vector>

namespace LaurelEye::Graphics {

    class SkeletonManager {
    public:
        SkeletonHandle createSkeleton(const LaurelEye::IO::SkeletonAsset& asset);
        const Skeleton* getSkeleton(SkeletonHandle h) const;
        Skeleton* getSkeleton(SkeletonHandle h);

        SkeletonHandle getHandleByName(const std::string& name) const {
            auto it = skeletonNames.find(name);
            return (it == skeletonNames.end()) ? Skeleton::InvalidSkeleton : it->second;
        }

        bool hasSkeleton(SkeletonHandle h) const {
            return isValidSkeleton(h) && h < skeletons.size();
        }

        void destroySkeleton(SkeletonHandle h);
        std::size_t getSkeletonCount() const { return skeletons.size(); }

    private:
        std::vector<Skeleton> skeletons;
        std::unordered_map<std::string, SkeletonHandle> skeletonNames;

        SkeletonHandle allocateSkeletonHandle(const std::string& name);
    };
} // namespace LaurelEye::Graphics
