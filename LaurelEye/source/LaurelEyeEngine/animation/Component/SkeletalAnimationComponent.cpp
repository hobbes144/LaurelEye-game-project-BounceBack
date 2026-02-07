#include "LaurelEyeEngine/animation/components/SkeletalAnimationComponent.h"

namespace LaurelEye::Animations {
    void SkeletalAnimationComponent::changeAnimation(const std::string name)
    {
        CurrentAnimationName = name;
        isChange = true;
    }
    void SkeletalAnimationComponent::setTime(float input)
    {
        elapsedTime = input;
    }
    void SkeletalAnimationComponent::setStart(bool input)
    {
        isPlaying = input;
    }
}
