#pragma once

// Notes:
//
// This will contain all the animation logic.
// Ideally we store the animation method in the config, so that we can
// change from VQS to iVQS or keyframe only or other methods in the future.
//
// During init of a component:
// Init the AnimationComponent with the bone localPose values.
//
// During setAnimation of a component:
// We set up the time related info, the animation resource being animated.
// The system will update the localPose of the AnimationComponent using the
// animation technique setup in the config. (Maybe allow this to be per comp?)
//
// During update of a component:
// Update the globalPose, then upload the data to GPU using relevant UBO.
//
// Ideally the Mesh draw will take care of the UBO bind during draw calls, so
// we could cache the UBO handle in the Mesh for efficiency.
//
// During Animation resource load:
// Create the resource with channels that map to the respective bone index.

#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/animation/components/AnimationComponent.h"
#include "components/SkeletalAnimationComponent.h"

namespace LaurelEye::Graphics {
    class RenderResources;
    class SkeletonManager;
}

namespace LaurelEye::Animations {

    class AnimationManager;

    struct AnimationSystemConfig {
        Graphics::RenderResources* renderResources;
        Graphics::SkeletonManager* skeletonManager;
    };

    class AnimationSystem : public ISystem<AnimationComponent> {
    public:
        /// \copydoc ISystem::initialize()
        void initialize() override;
        /// \copydoc ISystem::update
        void update(float deltaTime) override;
        /// \copydoc ISystem::shutdown
        void shutdown() override;

        void setConfig(const AnimationSystemConfig& config);

        void registerComponent(const ComponentPtr component) override;
        void deregisterComponent(const ComponentPtr component) override;

        AnimationSystem();  
        ~AnimationSystem();

        void setAnimation(ComponentPtr e, AnimationHandle anim);


    private:
        Graphics::RenderResources* renderResources;
        Graphics::SkeletonManager* skeletonManager;

        AnimationManager* animationManager;

        void updateSkeletalAnimations(float deltaTime, SkeletalAnimationComponent* comp);
    };
}
