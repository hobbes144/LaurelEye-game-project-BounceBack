#include "LaurelEyeEngine/core/SystemCoordinator.h"
#include "LaurelEyeEngine/window/WindowManager.h"

namespace LaurelEye {
    SystemCoordinator::SystemCoordinator(EngineContext& ctx, const EngineConfig& engineConfig) {
        std::cout << "Systems Constructing" << std::endl;
        // TODO - anything that needs ctx or engine config sets them here

        transformSystem = std::make_unique<TransformSystem>();
        transformSystem->setEngineContext(ctx);

        renderSystem = std::make_unique<Graphics::RenderSystem>();

        // Render system setup
        LaurelEye::Graphics::RenderSystemConfig renderConfig;
        WindowManager* windowManager = ctx.getService<WindowManager>();
        if ( windowManager ) {
            IWindow* currentWindow = ctx.getService<WindowManager>()->getWindow(0);
            if ( currentWindow ) {
                renderConfig.windows.push_back(currentWindow);
            }
            else {
                std::cerr << "Could not find Main Window in SystemCoordinator - Renderer will likely not work!" << std::endl;
            }
        }
        else {
            std::cerr << "Could not find WindowManager in SystemCoordinator - Renderer will likely not work!" << std::endl;
        }
        renderSystem->setConfig(renderConfig);
        renderSystem->setRunDebugDraw(engineConfig.enableDebugMode);

        particleSystem = std::make_unique<Particles::ParticleSystem>();
        particleSystem->setEngineContext(ctx);
        particleSystem->setMaxParticles(engineConfig.particles.MaxParticles);

        physicsSystem = std::make_unique<Physics::PhysicsSystem>();
        physicsSystem->setEngineContext(ctx);
        // Calls its own initialize and shutdown methods - in future will get reworked
        audioSystem = std::make_unique<Audio::AudioSystem>();
        audioSystem->setEngineContext(ctx);

        scriptSystem = std::make_unique<Scripting::ScriptSystem>();
        scriptSystem->setEngineContext(ctx);

        // If Debug Drawing is enabled, create and register the system
        if ( engineConfig.enableDebugMode ) {
            debugDrawSystem = std::make_unique<Debug::DebugDrawSystem>();
            debugDrawSystem->setEngineContext(ctx);
            ctx.registerService<Debug::DebugDrawSystem>(debugDrawSystem.get());
        }

        ctx.registerService<TransformSystem>(transformSystem.get());
        ctx.registerService<Graphics::RenderSystem>(renderSystem.get());
        ctx.registerService<Physics::PhysicsSystem>(physicsSystem.get());
        ctx.registerService<Particles::ParticleSystem>(particleSystem.get());
        ctx.registerService<Audio::AudioSystem>(audioSystem.get());
        ctx.registerService<Scripting::ScriptSystem>(scriptSystem.get());
    }

    void SystemCoordinator::initialize() {
        std::cout << "Systems Initializing" << std::endl;
        transformSystem->initialize();
        renderSystem->initialize();
        physicsSystem->initialize();
        scriptSystem->initialize();
        particleSystem->initialize();
        debugDrawSystem->initialize();
        audioSystem->initialize();
    }
    void SystemCoordinator::update(float deltaTime) {
        scriptSystem->update(deltaTime);
        transformSystem->update(deltaTime);
        // Moved physics here since Bullet takes care of variable timestep
        // itself. We just call it with the real deltaTime and that's enough.
        physicsSystem->update(deltaTime);
        transformSystem->update(deltaTime);
        particleSystem->update(deltaTime);
        audioSystem->update(deltaTime);
        if ( debugDrawSystem ) {
            debugDrawSystem->update(deltaTime);
        }
        renderSystem->update(deltaTime);
    }
    void SystemCoordinator::updateFixed(float deltaTimeFixed) {
        // transformSystem->update(deltaTimeFixed);
        // physicsSystem->update(deltaTimeFixed);
    }

    void SystemCoordinator::shutdown() {
        std::cout << "Systems Shutting Down" << std::endl;
        particleSystem->shutdown();
        scriptSystem->shutdown();
        physicsSystem->shutdown();
        renderSystem->shutdown();
        transformSystem->shutdown();
        debugDrawSystem->shutdown();
        audioSystem->shutdown();

        transformSystem.reset();
        renderSystem.reset();
        physicsSystem.reset();
        particleSystem.reset();
        audioSystem.reset();
        scriptSystem.reset();
        debugDrawSystem.reset();
    }
} // namespace LaurelEye
