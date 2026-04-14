#include "LaurelEyeEngine/core/SystemCoordinator.h"
#include "LaurelEyeEngine/window/WindowManager.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye {
    SystemCoordinator::SystemCoordinator(EngineContext& ctx, const EngineConfig& engineConfig) {
        LE_INFO("SystemCoordinator", "Systems Constructing");

        this->ctx = &ctx;

        transformSystem = std::make_unique<TransformSystem>();
        transformSystem->setEngineContext(ctx);

        uiLayoutSystem = std::make_unique<UI::UILayoutSystem>();
        uiLayoutSystem->setEngineContext(ctx);

        uiInteractionSystem = std::make_unique<UI::UIInteractionSystem>();
        uiInteractionSystem->setEngineContext(ctx);

        renderSystem = std::make_unique<Graphics::RenderSystem>();

        // Render system setup
        LaurelEye::Graphics::RenderSystemConfig renderConfig;
        WindowManager* windowManager = ctx.getService<WindowManager>();
        LE_DEBUG_WARN_IF("SystemCoordinator", !windowManager, "Could not find WindowManager in SystemCoordinator - Renderer will likely not work!");
        if ( windowManager ) {
            IWindow* currentWindow = ctx.getService<WindowManager>()->getWindow(0);
            LE_DEBUG_WARN_IF("SystemCoordinator", !currentWindow, "Could not find Main Window in SystemCoordinator - Renderer will likely not work!");
            if ( currentWindow ) {
                renderConfig.windows.push_back(currentWindow);
            }
        }

        renderSystem->setConfig(renderConfig);
        renderSystem->setRunDebugDraw(engineConfig.enableDebugMode);

        particleSystem = std::make_unique<Particles::ParticleSystem>();
        particleSystem->setEngineContext(ctx);
        particleSystem->setMaxParticles(engineConfig.particles.MaxParticles);

        physicsSystem = std::make_unique<Physics::PhysicsSystem>();
        physicsSystem->setEngineContext(ctx);
        // Calls its own initialize and shutdown methods - in future will get reworked
        audioSystem = std::make_unique<Audio::AudioSystem>(ctx.getService<Audio::IAudioManager>());
        audioSystem->setEngineContext(ctx);

        scriptSystem = std::make_unique<Scripting::ScriptSystem>();
        scriptSystem->setEngineContext(ctx);

        animationSystem = std::make_unique<Animations::AnimationSystem>();
        animationSystem->setEngineContext(ctx);

        // If Debug Drawing is enabled, create and register the system
        if ( engineConfig.enableDebugMode ) {
            debugDrawSystem = std::make_unique<Debug::DebugDrawSystem>();
            debugDrawSystem->setEngineContext(ctx);
            ctx.registerService<Debug::DebugDrawSystem>(debugDrawSystem.get());
        }

        ctx.registerService<TransformSystem>(transformSystem.get());
        ctx.registerService<UI::UILayoutSystem>(uiLayoutSystem.get());
        ctx.registerService<UI::UIInteractionSystem>(uiInteractionSystem.get());
        ctx.registerService<Graphics::RenderSystem>(renderSystem.get());
        ctx.registerService<Physics::PhysicsSystem>(physicsSystem.get());
        ctx.registerService<Particles::ParticleSystem>(particleSystem.get());
        ctx.registerService<Audio::AudioSystem>(audioSystem.get());
        ctx.registerService<Scripting::ScriptSystem>(scriptSystem.get());
        ctx.registerService<Animations::AnimationSystem>(animationSystem.get());
    }

    void SystemCoordinator::initialize() {
        LE_INFO("SystemCoordinator", "Systems Initializing");
        transformSystem->initialize();
        uiLayoutSystem->initialize();
        uiInteractionSystem->initialize();
        renderSystem->initialize();
        physicsSystem->initialize();
        scriptSystem->initialize();
        particleSystem->initialize();
        debugDrawSystem->initialize();
        audioSystem->initialize();
        animationSystem->initialize();
    }

    void SystemCoordinator::update() {
        scriptSystem->update(ctx->time().scaledDt);
        transformSystem->update(ctx->time().unscaledDt);
        // Moved physics here since Bullet takes care of variable timestep
        // itself. We just call it with the real deltaTime and that's enough.
#ifndef ENABLE_FC_PHYSICS_ACCUMULATOR
        physicsSystem->update(ctx->time().scaledDt);
#endif
        transformSystem->update(ctx->time().unscaledDt);
        uiLayoutSystem->update(ctx->time().unscaledDt);
        uiInteractionSystem->update(ctx->time().unscaledDt);
        particleSystem->update(ctx->time().scaledDt);
        audioSystem->update(ctx->time().scaledDt);
        if ( debugDrawSystem ) {
            debugDrawSystem->update(ctx->time().scaledDt);
        }
        physicsSystem->update(ctx->time().scaledDt);
        animationSystem->update(ctx->time().scaledDt);
        renderSystem->update(ctx->time().unscaledDt);
    }

#ifdef ENABLE_FC_PHYSICS_ACCUMULATOR
    void SystemCoordinator::updateFixed(float deltaTimeFixed) {
        transformSystem->update(deltaTimeFixed);
        physicsSystem->update(deltaTimeFixed);
    }
#endif

    void SystemCoordinator::shutdown() {
        LE_INFO("SystemCoordinator", "Systems Shutting Down");
        particleSystem->shutdown();
        scriptSystem->shutdown();
        physicsSystem->shutdown();
        renderSystem->shutdown();
        uiLayoutSystem->shutdown();
        uiInteractionSystem->shutdown();
        transformSystem->shutdown();
        debugDrawSystem->shutdown();
        audioSystem->shutdown();

        transformSystem.reset();
        uiLayoutSystem.reset();
        uiInteractionSystem.reset();
        renderSystem.reset();
        physicsSystem.reset();
        particleSystem.reset();
        audioSystem.reset();
        scriptSystem.reset();
        debugDrawSystem.reset();
    }
} // namespace LaurelEye
