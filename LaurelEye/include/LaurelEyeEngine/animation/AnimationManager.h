#pragma once

#include "LaurelEyeEngine/animation/resources/Animation.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <vector>
namespace LaurelEye::Animations {

    class AnimationManager {
    public:
        AnimationHandle createAnimation(const IO::AnimationAsset* asset);
        AnimationHandle getHandle(const std::string& name) const;

        const Animation* getAnimation(AnimationHandle h) const;
        Animation* getAnimation(AnimationHandle h);

        void destroyAnimation(AnimationHandle h);

        void destroyAllAnimations();

        ~AnimationManager();

    private:
        std::vector<std::unique_ptr<Animation>> animations;
        std::unordered_map<std::string, AnimationHandle> animationNames;
    };

} // namespace LaurelEye::Animations
