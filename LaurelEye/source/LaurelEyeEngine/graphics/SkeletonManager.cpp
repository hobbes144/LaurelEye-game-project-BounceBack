#include "LaurelEyeEngine/graphics/SkeletonManager.h"
#include "LaurelEyeEngine/graphics/resources/Skeleton.h"

namespace LaurelEye::Graphics {

    SkeletonHandle SkeletonManager::createSkeleton(const LaurelEye::IO::SkeletonAsset& asset) {
        SkeletonHandle handle = allocateSkeletonHandle(asset.getName());

        auto& skeleton = skeletons[handle];

        for ( const auto& bone : asset.bones ) {
            skeleton.bones.push_back(
                Skeleton::Bone{bone.name,
                               bone.localBindTransform});
        }
        skeleton.parentIndices = asset.parentIndices;
        skeleton.inverseTransform = asset.inverseTransform;
        skeleton.boneNameIndex = asset.boneNameIndex;

        return handle;
    }

    SkeletonHandle SkeletonManager::allocateSkeletonHandle(const std::string& name) {
        SkeletonHandle handle = static_cast<SkeletonHandle>(skeletons.size());
        skeletons.push_back(Skeleton{});
        skeletons.back().handle = handle;
        skeletons.back().name = name;
        skeletonNames[name] = handle;

        return handle;
    }

    const Skeleton* SkeletonManager::getSkeleton(SkeletonHandle h) const {
        return hasSkeleton(h) ? &skeletons[h] : nullptr;
    }

    Skeleton* SkeletonManager::getSkeleton(SkeletonHandle h) {
        return hasSkeleton(h) ? &skeletons[h] : nullptr;
    }

} // namespace LaurelEye::Graphics
