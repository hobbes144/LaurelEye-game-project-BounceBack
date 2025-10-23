#include "LaurelEyeEngine/core/Engine.h"
#include <iostream>

namespace LaurelEye {
    Engine::Engine(const EngineConfig& config) : engineConfig(config){}

    Engine::~Engine() {
        if ( isRunning ) {
            stop();
            shutdown();
        }
    }

    void Engine::run() {
        std::cout << "Engine Started" << std::endl;
        isRunning = true;
        initialize();

        while ( isRunning ) {
            // TODO - Determine deltaTime here using FrameRateController
            float deltaTime = 0.016;
            systemCoordinator->update(deltaTime);
            resourceCoordinator->update(deltaTime);
        }

        shutdown();
    }

    void Engine::stop() {
        isRunning = false;
    }

    void Engine::initialize() {
        
        ctx = std::make_unique<EngineContext>();

        resourceCoordinator = std::make_unique<ResourceCoordinator>();
        systemCoordinator = std::make_unique<SystemCoordinator>();

        resourceCoordinator->initialize(*ctx, engineConfig);
        systemCoordinator->initialize(*ctx, engineConfig);
    }

    void Engine::shutdown() {
        std::cout << "Engine Shutting down" << std::endl;
        systemCoordinator->shutdown();
        resourceCoordinator->shutdown();
    }

} // namespace LaurelEye
