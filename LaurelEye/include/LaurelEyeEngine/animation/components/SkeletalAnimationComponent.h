#pragma once

// Notes:
//
// Each Animation component will store a handle to the Animation resource.
// The animations themselves (with the per bone channel) will be stored in the
// resource, not here.
// AnimationHandle anim;
//
// The Animation Component needs to store the localPose and globalPose.
// std::vector<Transform> localPose;
// std::vector<Transform> globalPose;
//
// The indices of these correspond to the relative bone indices of the
// skeleton. This is because the final animation transform is calculated in
// GPU using the index order for skin to bone inverse transform and animation
// transform mapping.
// Skin to bone inverse is stored in the Mesh right now, but should be in a
// `Skin` resource later, which the Mesh has a handle to.
//
// localPose is the VQS or iVQS or whatever is used, it is calculated from the
// previous and next frame and time values.
//
// globalPose is the final animation transform calculated from the localPose
// and applied to the parentPose. The parentPose would be root inverse
// transform for root node, and each node will propagate its globalPose as its
// children's parentPose.
//
// Finally, the entire globalPose will be uploaded to the GPU via a UBO, so we
// need the UBO handle here as well. The best way to handle
// this for now is to have the RenderSystem always assign a globalPose UBO to
// the RenderComponent, and the Animation Component will always cache this on
// init and update the relevant UBO via RenderResources.
// DataBufferHandle buffer;

#include "LaurelEyeEngine/animation/components/AnimationComponent.h"

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Skeleton.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Transform.h"

namespace LaurelEye::Animations {

    struct SkeletalAnimationComponent : public AnimationComponent {
        SkeletalAnimationComponent() : AnimationComponent(Animation::Type::Skeletal) {}
        std::vector<Transform> localPose;
        std::vector<Matrix4> globalPose;
        Matrix4 inverseTransform;

        std::string CurrentAnimationName;
        std::string objName;
        bool isChange = false;

        Graphics::DataBufferHandle AnimationDataBuffer;
        Graphics::SkeletonHandle skeleton;

        // this is bad, but it seems everyone is doing it
        void changeAnimation(const std::string name);

        void setTime(float input); // Animation time
        void setStart(bool input); // turn on and off

    };

} // namespace LaurelEye::Animations
