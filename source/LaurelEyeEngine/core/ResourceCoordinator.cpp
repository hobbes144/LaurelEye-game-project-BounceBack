#include "LaurelEyeEngine/core/ResourceCoordinator.h"
#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"

namespace LaurelEye {
    ResourceCoordinator::ResourceCoordinator(EngineContext& ctx, const EngineConfig& engineConfig) {
        std::cout << "Resources Constructing" << std::endl;

        // TODO - let Anish at this to make it modular, just getting window up for now
        platformManager = std::make_unique<LaurelEye::GlfwPlatform>();
        platformManager->initialize();

        windowManager = std::make_unique<WindowManager>();
        auto window = windowManager->createWindow(LaurelEye::WindowDescription());

        // TODO - anything that needs ctx or engine config sets them here
        inputManager = std::make_unique<InputManager>(*window);
        memoryManager = std::make_unique<MemoryManager>();
        assetManager = std::make_unique<IO::AssetManager>(ctx);
        sceneManager = std::make_unique<SceneManager>(ctx, engineConfig);
        eventManager = std::make_unique<EventManager>();

        ctx.registerService<InputManager>(inputManager.get());
        ctx.registerService<WindowManager>(windowManager.get());
        ctx.registerService<MemoryManager>(memoryManager.get());
        ctx.registerService<IO::AssetManager>(assetManager.get());
        ctx.registerService<EventManager>(eventManager.get());
        ctx.registerService<SceneManager>(sceneManager.get());
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
        inputManager.reset();
        windowManager.reset();
        memoryManager.reset();
        assetManager.reset();
        sceneManager.reset();
    }
} // namespace LaurelEye
