/// @file   SystemCoordinator.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  One of the "core" functionalities of our engine, and held by Engine.h
/// Manages the loading of, updating, and shutting down of systems active in the system.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/audio/AudioSystem.h"
#include "LaurelEyeEngine/core/Config.h"
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/debugDraw/DebugDrawSystem.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/particles/ParticleSystem.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/transform/TransformSystem.h"
#include "LaurelEyeEngine/animation/AnimationSystem.h"
#include "LaurelEyeEngine/UI/UILayoutSystem.h"
#include "LaurelEyeEngine/UI/UIInteractionSystem.h"
#include <memory>

namespace LaurelEye {
    class SystemCoordinator {
    public:
        SystemCoordinator(EngineContext& ctx, const EngineConfig& engineConfig);
        void initialize();
        void update(float deltaTime);
        void updateFixed(float deltaTimeFixed);
        void shutdown();

    private:
        // TODO - determine if this should be generic list
        // for now, add systems that are needed here.
        std::unique_ptr<TransformSystem> transformSystem;
        std::unique_ptr<UI::UILayoutSystem> uiLayoutSystem;
        std::unique_ptr<UI::UIInteractionSystem> uiInteractionSystem;
        std::unique_ptr<Graphics::RenderSystem> renderSystem;
        std::unique_ptr<Particles::ParticleSystem> particleSystem;
        std::unique_ptr<Physics::PhysicsSystem> physicsSystem;
        std::unique_ptr<Audio::AudioSystem> audioSystem;
        std::unique_ptr<Scripting::ScriptSystem> scriptSystem;
        std::unique_ptr<Debug::DebugDrawSystem> debugDrawSystem;
        std::unique_ptr<Animations::AnimationSystem> animationSystem;
        // ... add more systems as needed
    };
} // namespace LaurelEye
