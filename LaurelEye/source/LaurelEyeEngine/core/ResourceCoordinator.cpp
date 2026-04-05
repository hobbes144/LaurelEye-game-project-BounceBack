#include "LaurelEyeEngine/audio/FModAudioManager.h"
#include "LaurelEyeEngine/core/ResourceCoordinator.h"
#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"
#include "LaurelEyeEngine/window/IWindow.h"

namespace LaurelEye {
    ResourceCoordinator::ResourceCoordinator(EngineContext& ctx, const EngineConfig& engineConfig) {
        std::cout << "Resources Constructing" << std::endl;

        // TODO - let Anish at this to make it modular, just getting window up for now
        platformManager = std::make_unique<LaurelEye::GlfwPlatform>();
        platformManager->initialize();

        windowManager = std::make_unique<WindowManager>();
        LaurelEye::WindowDescription windowDesc{};
        windowDesc.width = engineConfig.window.width;
        windowDesc.height = engineConfig.window.height;
        windowDesc.windowedX = engineConfig.window.windowedX;
        windowDesc.windowedY = engineConfig.window.windowedY;
        windowDesc.mode = engineConfig.window.fullscreen ? WindowMode::Borderless : WindowMode::Windowed;
        windowDesc.title = engineConfig.window.title;
        windowDesc.vsync = engineConfig.window.vsync;
        auto window = windowManager->createWindow(windowDesc);

        // TODO - anything that needs ctx or engine config sets them here
        inputManager = std::make_unique<InputManager>(*window);
        memoryManager = std::make_unique<MemoryManager>();
        assetManager = std::make_unique<IO::AssetManager>(ctx);
        sceneManager = std::make_unique<SceneManager>(ctx, engineConfig);
        eventManager = std::make_unique<EventManager>();
        animationManager = std::make_unique<Animations::AnimationManager>();
        FModAudioManager = std::make_unique<Audio::FModAudioManager>();
        FModAudioManager->initialize();
        gameManager = std::make_unique<GameManager>();

        ctx.registerService<InputManager>(inputManager.get());
        ctx.registerService<WindowManager>(windowManager.get());
        ctx.registerService<MemoryManager>(memoryManager.get());
        ctx.registerService<IO::AssetManager>(assetManager.get());
        ctx.registerService<EventManager>(eventManager.get());
        ctx.registerService<SceneManager>(sceneManager.get());
        ctx.registerService<Animations::AnimationManager>(animationManager.get());
        ctx.registerService<Audio::IAudioManager>(FModAudioManager.get());
        ctx.registerService<GameManager>(gameManager.get());
    }

    void ResourceCoordinator::initialize() {
        std::cout << "Resources Initializing" << std::endl;
        assetManager->initialize(); // sets extensions to assets
        sceneManager->initialize(); // Sets initial scene
    }

    void ResourceCoordinator::update(float deltaTime) {
        platformManager->update();
        inputManager->update();
        sceneManager->update(deltaTime);
    }

    void ResourceCoordinator::shutdown() {
        std::cout << "Resources Shutting Down" << std::endl;
        sceneManager.reset();
        inputManager.reset();
        windowManager.reset();
        memoryManager.reset();
        assetManager.reset();
        animationManager.reset();
        gameManager.reset();
    }
} // namespace LaurelEye
