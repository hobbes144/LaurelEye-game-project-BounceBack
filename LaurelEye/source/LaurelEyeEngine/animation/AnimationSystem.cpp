#include "LaurelEyeEngine/animation/AnimationSystem.h"
#include "LaurelEyeEngine/animation/AnimationManager.h"
#include "LaurelEyeEngine/animation/resources/SkeletalAnimation.h"
#include "LaurelEyeEngine/animation/AnimationHelpers.h"
#include "LaurelEyeEngine/graphics/SkeletonManager.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/MeshManager.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <memory>

namespace LaurelEye::Animations {

    AnimationSystem::AnimationSystem() = default;
    AnimationSystem::~AnimationSystem() = default;

    void AnimationSystem::setConfig(const AnimationSystemConfig& config) {
        renderResources = config.renderResources;
        skeletonManager = config.skeletonManager;
    }

    void AnimationSystem::initialize() {
        if ( !context ) {
            std::cerr << "AnimationSystem::initialize - No context available\n";
            return;
        }

        animationManager = context->getService<AnimationManager>();
        if ( !animationManager ) {
            std::cerr << "AnimationSystem::initialize - Failed to get AnimationManager\n";
        }

        // Render System
        auto renderSystem = context->getService<LaurelEye::Graphics::RenderSystem>();
        if ( !renderSystem ) {
            std::cerr << "AnimationSystem::initialize - Failed to get RenderSystem\n";
            return; // cannot continue without it
        }

        // Skeleton Manager
        skeletonManager = renderSystem->getSkeletonManager();
        if ( !skeletonManager ) {
            std::cerr << "AnimationSystem::initialize - Failed to get SkeletonManager\n";
        }

        // Render Resources
        renderResources = renderSystem->getRenderResources();
        if ( !renderResources ) {
            std::cerr << "AnimationSystem::initialize - Failed to get RenderResources\n";
        }
    }



    void AnimationSystem::shutdown() {
        // clear manager, exit;
        renderResources = nullptr;
        skeletonManager = nullptr;
    }

    void AnimationSystem::registerComponent(const ComponentPtr component) {
        // Register with the base ISystem list
        LaurelEye::ISystem<AnimationComponent>::registerComponent(component);

        auto* renderable = component->getOwner()->findComponent<LaurelEye::Graphics::IRenderableComponent>();
        if ( !renderable ) {
            // need a renderable to get the mesh asset
            return;
        }
        auto MeshHandle = renderable->GetMesh();
        auto renderSystem = context->getService<LaurelEye::Graphics::RenderSystem>();
        auto meshAsset = renderSystem->getMeshManager()->getMesh(MeshHandle);


        auto* base = dynamic_cast<AnimationComponent*>(component);

        // Check if this is a skeletal animation component
        if ( auto* skeletal = dynamic_cast<SkeletalAnimationComponent*>(base) ) {
            skeletal->skeleton = meshAsset->skeleton;

            // Fetch skeleton data
            Graphics::SkeletonHandle skeletonHandle = skeletal->skeleton;

            auto* skeletonData = skeletonManager->getSkeleton(skeletonHandle);
            if (!skeletonData) {
                return;
            }

            const size_t boneCount = skeletonData->getBoneCount();

            // Allocate pose buffers
            skeletal->localPose.resize(boneCount);
            skeletal->globalPose.resize(boneCount);
            skeletal->inverseTransform = skeletonData->inverseTransform.getLocalMatrix();

            // Setup AnimationData for buffer
            std::vector<Matrix4> globalBindPose(boneCount);

            // build global bind pose from skeleton.localBindTransform
            for (size_t i = 0; i < boneCount; ++i ) {
                int parent = skeletonData->parentIndices[i];
                Matrix4 local = skeletonData->bones[i].localBindTransform.getMatrix();

                globalBindPose[i] = (parent < 0)
                                      ? skeletonData->inverseTransform.getLocalMatrix() * local
                                      : globalBindPose[parent] * local;
            }

            // Allocate GPU buffer for animation matrices
            skeletal->AnimationDataBuffer = renderResources->createDataBuffer(
                    component->getOwner()->getName() + ":AnimData",
                    LaurelEye::Graphics::DataBufferDesc{
                        LaurelEye::Graphics::DataBufferType::SSBO,
                        LaurelEye::Graphics::DataBufferUpdateMode::Dynamic,
                        sizeof(Matrix4) * boneCount,
                        LaurelEye::Graphics::DataBuffer::AnimationDataBinding},
                    "meshManager:animationData",
                    globalBindPose.data());

            renderable->setAnimationDataBuffer(skeletal->AnimationDataBuffer);

            return;
        }
    }

    void AnimationSystem::deregisterComponent(const ComponentPtr component)
    {
        ISystem::deregisterComponent(component);
    };

    void AnimationSystem::update(float deltaTime) {
        // return;
        // Update base elapsed time for all animations.
        for ( auto& comp : components ) {
            if ( !isValidAnimation(comp->currentAnimation) ) continue;
            if ( !comp->isActive() ) continue;
            comp->elapsedTime += static_cast<double>(deltaTime);
            if ( auto* skeletal = dynamic_cast<SkeletalAnimationComponent*>(comp) ) { updateSkeletalAnimations(deltaTime, skeletal); }
        }
    }

    void AnimationSystem::updateSkeletalAnimations(float deltaTime, SkeletalAnimationComponent* comp) {
        if ( comp->isPlaying == false ) return;
        if ( comp->isChange == true ) {
            comp->currentAnimation = animationManager->getHandle(comp->CurrentAnimationName);
            comp->isChange = false;
            comp->elapsedTime = 0.0;
        }
        LaurelEye::Graphics::Skeleton* skeRef = skeletonManager->getSkeleton(comp->skeleton);
        if ( skeRef == nullptr ) {
            return;
        }

        Animation* animation = animationManager->getAnimation(comp->currentAnimation);
        SkeletalAnimation* skelAnimation = dynamic_cast<SkeletalAnimation*>(animation);
        if ( !animation ) {
            return;
        }

        const size_t boneCount = skeRef->parentIndices.size();

        // Convert elapsed time -> animation time
        double timeInTicks = comp->elapsedTime * skelAnimation->ticksPerSecond;
        float animTime = static_cast<float>(wrapTime(timeInTicks, skelAnimation->duration));

        // Update local transforms for each bone
        for ( size_t i = 0; i < boneCount; i++ ) {
            if ( i < skelAnimation->channels.size() ) {
                comp->localPose[i] = sampleChannel(skelAnimation->channels[i], animTime);
            }
            else {
                // No animation channel -> use bind pose
                comp->localPose[i] = skeRef->bones[i].localBindTransform;
            }
        }

        // it seems like it does not have the update on the skeleton
        // Update global transforms based on hierarchy
        for ( size_t i = 0; i < boneCount; i++ ) {
            int parent = skeRef->parentIndices[i];

            if ( parent < 0 ) {
                // Root bone
                comp->globalPose[i] = comp->inverseTransform * comp->localPose[i].getMatrix();
            }
            else {
                comp->globalPose[i] =
                    comp->globalPose[parent] *
                    comp->localPose[i].getMatrix();
            }
        }

        // Upload updated global pose to GPU buffer
        renderResources->updateDataBuffer(
            comp->AnimationDataBuffer,   // SSBO handle
            0,                           // offset in bytes
            sizeof(Matrix4) * boneCount, // size in bytes
            comp->globalPose.data()      // pointer to CPU data
        );

    }

} // namespace LaurelEye::Animations
