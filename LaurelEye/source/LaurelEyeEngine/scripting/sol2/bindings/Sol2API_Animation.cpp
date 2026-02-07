#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Animation.h"
#include "LaurelEyeEngine/animation/components/SkeletalAnimationComponent.h"

namespace LaurelEye::Scripting {
    void Sol2API_Animation::setup(sol::state& lua, EngineContext* ctx) {
        // --- Bind SkeletalAnimationComponent ---
        lua.new_usertype<Animations::SkeletalAnimationComponent>(
            "SkeletalAnimationComponent",

            // Constructors
            sol::constructors<
                Animations::SkeletalAnimationComponent()>(),

            // --- Animation Controls ---
            "changeAnimation", &Animations::SkeletalAnimationComponent::changeAnimation,
            "setTime", &Animations::SkeletalAnimationComponent::setTime,
            "setStart", &Animations::SkeletalAnimationComponent::setStart,

            // all these below should not be use, but put it here just in case for debugging or future use, since they are not really harmful to expose
            // --- Animation State ---
            "currentAnimationName", &Animations::SkeletalAnimationComponent::CurrentAnimationName,
            "objectName", &Animations::SkeletalAnimationComponent::objName,
            "isChange", &Animations::SkeletalAnimationComponent::isChange,

            // --- Pose Data (read/write vectors) ---
            "localPose", &Animations::SkeletalAnimationComponent::localPose,
            "globalPose", &Animations::SkeletalAnimationComponent::globalPose,

            // --- Transform Data ---
            "inverseTransform", &Animations::SkeletalAnimationComponent::inverseTransform,

            // --- GPU / Skeleton Handles ---
            "animationBuffer", &Animations::SkeletalAnimationComponent::AnimationDataBuffer,
            "skeleton", &Animations::SkeletalAnimationComponent::skeleton);

    }
} // namespace LaurelEye::Scripting
