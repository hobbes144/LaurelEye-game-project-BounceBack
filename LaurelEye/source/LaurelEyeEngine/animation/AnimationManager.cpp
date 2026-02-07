#include "LaurelEyeEngine/animation/AnimationManager.h"
#include <LaurelEyeEngine/animation/resources/SkeletalAnimation.h>

namespace LaurelEye::Animations {
    AnimationHandle AnimationManager::createAnimation(const IO::AnimationAsset* asset) {
        if ( !asset )
            return Animation::InvalidAnimation;

        // Prevent duplicates
        auto it = animationNames.find(asset->animName);
        if ( it != animationNames.end() )
            return it->second;

        auto anim = std::make_unique<SkeletalAnimation>();
        anim->name = asset->animName;
        anim->ticksPerSecond = asset->ticksPerSecond;
        anim->channels.resize(asset->channels.size());
        anim->duration = asset->duration;

        // Copy channels + keyframes
        for ( size_t c = 0; c < asset->channels.size(); ++c ) {
            const auto& srcChannel = asset->channels[c];
            auto& dstChannel = anim->channels[c];
            size_t count = srcChannel.keyframe.size();
            dstChannel.keyframes.reserve(count);
            for ( size_t i = 0; i < count; ++i ) {
                SkeletalAnimation::Keyframe kf;
                kf.localPose = srcChannel.keyframe[i];
                kf.timeStamp = static_cast<float>(srcChannel.timeStamp[i]);
                dstChannel.keyframes.push_back(kf);
            }
        }

        // Store animation and return handle
        AnimationHandle handle = static_cast<AnimationHandle>(animations.size());
        animationNames[asset->animName] = handle;
        animations.push_back(std::move(anim));

        return handle;
    }

    AnimationHandle AnimationManager::getHandle(const std::string& name) const {
        auto it = animationNames.find(name);
        if ( it == animationNames.end() )
            return Animation::InvalidAnimation;
        return it->second;
    }

    const Animation* AnimationManager::getAnimation(AnimationHandle h) const {
        if ( h >= animations.size() )
            return nullptr;
        return animations[h].get();
    }

    Animation* AnimationManager::getAnimation(AnimationHandle h) {
        if ( h >= animations.size() )
            return nullptr;
        return animations[h].get();
    }

    void AnimationManager::destroyAnimation(AnimationHandle h) {
        if ( h >= animations.size() )
            return;

        // Remove name -> handle mapping
        for ( auto it = animationNames.begin(); it != animationNames.end(); ++it ) {
            if ( it->second == h ) {
                animationNames.erase(it);
                break;
            }
        }

        // Reset the unique_ptr (deletes the old animation, sets to nullptr)
        animations[h].reset(); // Changed: use .reset() instead of assignment
    }

    void AnimationManager::destroyAllAnimations() {
        animations.clear(); // unique_ptr automatically deletes all animations
        animationNames.clear();
    }

    AnimationManager::~AnimationManager() {
        // Actually don't need to call destroyAllAnimations()
        // But keeping for clarity
        destroyAllAnimations();
    }


}

