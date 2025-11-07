/// @file   ResourceCoordinator.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   9-30-2025
/// @brief  One of the "core" functionalities of our engine, and held by Engine.h
/// Manages dependencies for and loads IResourceManagers to be used by systems.
/// 
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <vector>
#include <memory>
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/core/Config.h"
#include "LaurelEyeEngine/input/InputManager.h"
#include "LaurelEyeEngine/window/WindowManager.h"
#include "LaurelEyeEngine/memory/MemoryManager.h"
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/scene/SceneManager.h"
#include "LaurelEyeEngine/events/EventManager.h"
#include "LaurelEyeEngine/platforms/IPlatform.h"
#include "LaurelEyeEngine/audio/FModAudioManager.h"

namespace LaurelEye {
    class ResourceCoordinator {
    public:
        ResourceCoordinator(EngineContext& ctx, const EngineConfig& config);
        void initialize();
        // TODO - I'm putting this for input. If input needs update, it should be a system plz
        void update(float deltaTime);
        void shutdown();

    private:
        // TODO - determine if this should be generic list
        // for now, add managers that are needed here.
        std::unique_ptr<InputManager> inputManager;
        std::unique_ptr<WindowManager> windowManager;
        std::unique_ptr<MemoryManager> memoryManager;
        std::unique_ptr<IO::AssetManager> assetManager;
        std::unique_ptr<SceneManager> sceneManager;
        std::unique_ptr<EventManager> eventManager;
        std::unique_ptr<IPlatform> platformManager;
        std::unique_ptr<Audio::FModAudioManager> audioManager;
    };
} // namespace LaurelEye
