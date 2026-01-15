#pragma once

#include "LaurelEyeEngine/math/Transform.h"

#include <unordered_map>

namespace LaurelEye::Graphics {

    using SkeletonHandle = uint32_t;

    /// @class Skeleton
    /// @brief Mesh skeleton for complex animated objects
    ///
    /// Note for import:
    /// Parents should always appear before children in the array.
    struct Skeleton {
    public:
        static constexpr SkeletonHandle InvalidSkeleton = UINT32_MAX;

        SkeletonHandle handle = Skeleton::InvalidSkeleton;

        /// @class Bone
        /// @brief Bones of the Skeleton.
        ///
        /// Animation component will use localBind to reset to its rest pose.
        /// (Only when no animation is playing and init of component.)
        ///
        /// During Animation, the bones are largely untouched. We only
        /// really touch this for init and as a reference for the
        /// Animation and Skin's bones. And debug stuff.
        struct Bone {
            /// @brief Name of the bone.
            std::string name;
            /// @brief Local space bind pose relative to parent in Transform form.
            Transform lobalBindTransform;

            // Add the following back if ever needed.
            // /// @brief Local space bind pose
            // Matrix4 localBind;
            // /// @brief Global space bind pose
            // Matrix4 globalBind;
        };

        /// @brief Skeleton name.
        std::string name;

        // @brief Bones in the Skeleton.
        //
        // We are storing it in a vector to make it more efficient to traverse.
        std::vector<Bone> bones;
        /// @brief Parent indices for each bone.
        ///
        /// This is the most important thing here. It orders the bones and
        /// is used by the Animation component every frame to update the
        /// animations of the bone hierarchy correctly.
        ///
        ///
        /// For debugging:
        /// parentIndices.size() should always be equal to bones.size()
        std::vector<int> parentIndices;

        // This is useful for skeleton debug drawing. We will skip for now.
        // std::vector<std::pair<int, int>> boneEdges;

        /// @brief Bone names to the indices.
        ///
        /// This provides easy access to the bone indices via name. Useful for
        /// init of the animation channels, which is done via bone names as
        /// reference.
        std::unordered_map<std::string, int> boneNameIndex;

        int getBoneIndex(const std::string& name) const {
            auto it = boneNameIndex.find(name);
            return (it == boneNameIndex.end()) ? -1 : it->second;
        }

        std::size_t getBoneCount() const {
            return bones.size();
        }
    };

    inline bool isValidSkeleton(SkeletonHandle h) noexcept {
        return h != Skeleton::InvalidSkeleton;
    }

} // namespace LaurelEye::Graphics
