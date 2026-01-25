/// @file   Config.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  A purely data defining file for top level configs of the Engine.
/// I.e., EngineConfig
/// These will be defined by data files
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <iostream>

namespace LaurelEye {
    struct WindowConfig {
        int width = 1280;
        int height = 720;
        bool fullscreen = false;
        bool resizable = true;
        bool highDPISupport = true;
        int monitorIndex = 0; // which monitor to use
        std::string title = "LaurelEye Project";
    };

    struct RenderConfig {
        bool vsync = true;
        bool enableHDR = false;
        int targetFramerate = 60;

        bool enablePostProcessing = true;
        bool enableShadows = true;
        bool enableMSAA = false;
        int msaaSamples = 4;

        std::string renderBackend = "OpenGL"; // e.g., "Vulkan" or "DX12" later
        std::string defaultMaterial = "materials/default.mat";
        std::string defaultSkybox = "textures/skybox_default.hdr";
    };

    struct PhysicsConfig {
        float fixedDeltaTime = 0.0083333f;
        bool enableDebugDraw = false;
        std::string backend = "Bullet";
        int solverIterations = 10;
        bool enableCCD = false; // continuous collision detection
    };

    struct ParticleConfig {
        int MaxParticles = 5000;
    };

    struct AudioConfig {
        int sampleRate = 48000;
        int numChannels = 2;
        bool spatialAudio = true;
        std::string backend = "FMOD"; // or "OpenAL", etc.
    };

    struct DebugDrawConfig {
        unsigned int MaxLines = 5000;
        std::string HotKey = "Tilde";
    };

    struct EngineConfig {
        std::string projectName;
        std::string assetRoot;
        std::string initialSceneList; // e.g. "scenes/scene_list.json"
        bool enableDebugMode = false;

        WindowConfig window;
        RenderConfig render;
        PhysicsConfig physics;
        ParticleConfig particles;
        AudioConfig audio;
        DebugDrawConfig debugDraw;
    };

} // namespace LaurelEye
