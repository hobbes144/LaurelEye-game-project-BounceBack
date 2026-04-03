/// @file   Engine.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   9-30-2025
/// @brief  Main API that an external project will interact with to use our engine.
/// Namely, the engine can be started, ticked, and shutdown using this layer.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

#include "LaurelEyeEngine/core/Config.h"
#include "LaurelEyeEngine/core/EngineContext.h"

#include <memory>

namespace LaurelEye {

    class IWindow;
    class ResourceCoordinator;
    class SystemCoordinator;

    class Engine {
    public:
        Engine(const EngineConfig& config);
        ~Engine();

        /// @brief Begins engine execution, allows it to handle starting and stopping itself
        void run();
        /// @brief Allows outside stoppage of the engine if needed
        void stop();

        // Return the engine context stored here
        EngineContext& getContext() { return *ctx; }

        void setTimeScale(float timeScale);
        float getTimeScale() const;

    private:
        /// @brief Initialize the engine
        void initialize();
        /// @brief Shuts down engine cleanly
        void shutdown();

        // Local instance of the config for the engine
        EngineConfig engineConfig;
        // A structure of pointers to the instances derived from the coordinators
        std::unique_ptr<EngineContext> ctx;
        // Handles lifecycles and interfacing for "resources", i.e. resource managers.
        std::unique_ptr<ResourceCoordinator> resourceCoordinator;
        // Handles lifecycles, interfacing, and ticking for systems
        std::unique_ptr<SystemCoordinator> systemCoordinator;

        // For window lifecycle
        // TODO - this is set once and static.
        // We should have an event listener for window changes
        // in order to set this dynamically
        IWindow* currentWindow;

        // Whether or not the engine is actively running
        bool isRunning = false;
    };
} // namespace LaurelEye
