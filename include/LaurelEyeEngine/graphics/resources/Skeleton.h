#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include <map>

namespace LaurelEye::Graphics {

    /// @class Skeleton
    /// @brief Mesh skeleton for complex animated objects
    ///
    class Skeleton {
    public:

        /// @class Bone
        /// @brief Bones of the Skeleton in Node Tree structure.
        ///
        /// LocalTransform: Bind pose Transform of the bone. Not expected to
        /// change.
        /// WorldTransform: Transform after applying all the animation.
        ///
        /// Since LocalTransform doesn't change, we can fully ignore the
        /// dirty flag and resolving inverse transforms. We only ever forward
        /// propagate.
        ///
        /// During animation propagation, we will update the WorldTransform and
        /// then propagate the parent WorldTransform * AnimationMatrix.
        ///
        /// All this is done in the Animation resource, not here, so we only
        /// care about storing the two transforms.
        ///
        class Bone : public TransformComponent {};

        void updateBoneData(RenderResources& rs) const;

    private:

        /// @brief Skeleton name.
        std::string name;
        /// @brief Skeleton Root.
        Bone* rootBone;

        // @brief Bones in the Skeleton.
        //
        // We are storing it in a vector to make it more efficient when we
        // are uploading the bone data to the GPU.
        //
        // NOTE: Ideally the GPU uplopad should be handled with a persistent
        // buffer, but we don't have that implemented yet.
        //
        std::vector<Bone> bones;
        std::vector<std::pair<int, int>> boneEdges;
        std::map<std::string, int> boneNameIndex;

        DataBufferHandle boneTransformBuffer;

    };

} // namespace LaurelEye::Graphics
