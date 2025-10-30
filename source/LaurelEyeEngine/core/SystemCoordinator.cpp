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

        physicsSystem = std::make_unique<Physics::PhysicsSystem>();
        physicsSystem->setEngineContext(ctx);
        // Calls its own initialize and shutdown methods - in future will get reworked
        audioSystem = std::make_unique<Audio::FModAudioManager>();

        scriptSystem = std::make_unique<Scripting::ScriptSystem>();
        scriptSystem->setEngineContext(ctx);

        ctx.registerService<TransformSystem>(transformSystem.get());
        ctx.registerService<Graphics::RenderSystem>(renderSystem.get());
        ctx.registerService<Physics::PhysicsSystem>(physicsSystem.get());
        ctx.registerService<Audio::FModAudioManager>(audioSystem.get());
        ctx.registerService<Scripting::ScriptSystem>(scriptSystem.get());
    }

    void SystemCoordinator::initialize() {
        std::cout << "Systems Initializing" << std::endl;
        transformSystem->initialize();
        renderSystem->initialize();
        physicsSystem->initialize();
        scriptSystem->initialize();
    }
    void SystemCoordinator::update(float deltaTime) {
        //std::cout << "SysCord Update\n";
        transformSystem->update(deltaTime);
        renderSystem->update(deltaTime);
        audioSystem->update();
        scriptSystem->update(deltaTime);
    }
    void SystemCoordinator::updateFixed(float deltaTimeFixed) {
        physicsSystem->update(deltaTimeFixed);
    }

    void SystemCoordinator::shutdown() {
        std::cout << "Systems Shutting Down" << std::endl;
        transformSystem->shutdown();
        renderSystem->shutdown();
        physicsSystem->shutdown();
        scriptSystem->shutdown();

        transformSystem.reset();
        renderSystem.reset();
        physicsSystem.reset();
        audioSystem.reset();
        scriptSystem.reset();
    }
} // namespace LaurelEye
