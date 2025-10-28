#include "LaurelEyeEngine/core/Engine.h"
#include "LaurelEyeEngine/framerate/FramerateController.h"
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

        auto* frameController = FFramerateController::getController();
        frameController->setTargetFramerate(engineConfig.render.targetFramerate);
        frameController->setPhysicsTimestep(engineConfig.physics.fixedDeltaTime);

        while ( isRunning ) {
            frameController->startFrame();

            // --- Fixed timestep physics loop ---
            while ( frameController->shouldUpdatePhysics() ) {
                float physicsDelta = frameController->getPhysicsTimestep();
                //std::cout << "|Fixed\n";
                systemCoordinator->updateFixed(physicsDelta);
                frameController->consumePhysicsTime();
            }


            // --- Variable timestep updates (optional) ---
            float deltaTime = frameController->getFrameTime();
            //std::cout << "-Update\n";
            systemCoordinator->update(deltaTime);
            resourceCoordinator->update(deltaTime);

            // --- End frame and regulate FPS ---
            frameController->endFrame();

        }

        shutdown();
    }

    void Engine::stop() {
        isRunning = false;
    }

    void Engine::initialize() {
        
        ctx = std::make_unique<EngineContext>();

        resourceCoordinator = std::make_unique<ResourceCoordinator>(*ctx, engineConfig);
        systemCoordinator = std::make_unique<SystemCoordinator>(*ctx, engineConfig);

        resourceCoordinator->initialize();
        systemCoordinator->initialize();
    }

    void Engine::shutdown() {
        std::cout << "Engine Shutting down" << std::endl;
        systemCoordinator->shutdown();
        resourceCoordinator->shutdown();
    }

} // namespace LaurelEye
