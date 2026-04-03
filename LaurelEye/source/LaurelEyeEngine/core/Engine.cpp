#include "LaurelEyeEngine/core/Engine.h"
#include "LaurelEyeEngine/io/Paths.h"
#include "LaurelEyeEngine/core/ResourceCoordinator.h"
#include "LaurelEyeEngine/core/SystemCoordinator.h"
#include "LaurelEyeEngine/framerate/FramerateController.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

#include <iostream>

namespace LaurelEye {
    Engine::Engine(const EngineConfig& config) : engineConfig(config) {}

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
#ifdef ENABLE_FC_PHYSICS_ACCUMULATOR
        frameController->setPhysicsTimestep(engineConfig.physics.fixedDeltaTime);
#endif

        while ( isRunning && !currentWindow->shouldClose() ) {
            frameController->startFrame();
            LaurelEye::Log::set_frame(frameController->getFrameCount());

            // Time update
            float deltaTime = frameController->getFrameTime();
            auto& time = ctx->mutableTime();
            time.unscaledDt = deltaTime;
            time.scaledDt = time.timeScale * time.unscaledDt;
            time.unscaledTime += time.unscaledDt;
            time.scaledTime += time.scaledDt;

#ifdef ENABLE_FC_PHYSICS_ACCUMULATOR
            // --- Fixed timestep physics loop ---
            while ( frameController->shouldUpdatePhysics() ) {
                float physicsDelta = frameController->getPhysicsTimestep();
                systemCoordinator->updateFixed(physicsDelta * timeScale);
                frameController->consumePhysicsTime();
            }
#endif

            // --- Variable timestep updates (optional) ---

            systemCoordinator->update();
            resourceCoordinator->update(deltaTime);

            // --- End frame and regulate FPS ---
            frameController->endFrame();
        }
        stop();
        shutdown();
    }

    void Engine::stop() {
        isRunning = false;
    }

    void Engine::initialize() {

        ctx = std::make_unique<EngineContext>();
        ctx->registerService<Engine>(this);

        resourceCoordinator = std::make_unique<ResourceCoordinator>(*ctx, engineConfig);
        systemCoordinator = std::make_unique<SystemCoordinator>(*ctx, engineConfig);

        systemCoordinator->initialize();
        resourceCoordinator->initialize();

        currentWindow = ctx->getService<WindowManager>()->getWindow(0);
        if ( !currentWindow ) throw std::logic_error("No window registered to Engine, closing.");
    }

    void Engine::shutdown() {
        std::cout << "Engine Shutting down" << std::endl;
        currentWindow = nullptr;
        systemCoordinator->shutdown();
        resourceCoordinator->shutdown();
    }

    void Engine::setTimeScale(float timeScale) {
        ctx->mutableTime().timeScale = timeScale;
    }

    float Engine::getTimeScale() const {
        return ctx->time().timeScale;
    }

} // namespace LaurelEye
