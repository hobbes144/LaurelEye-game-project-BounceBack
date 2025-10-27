#include "LaurelEyeEngine/core/ResourceCoordinator.h"
#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"

namespace LaurelEye {
    void ResourceCoordinator::initialize(EngineContext& ctx, const EngineConfig& engineConfig) {
        std::cout << "Resources Initializing" << std::endl;

        // TODO - let Anish at this to make it modular, just getting window up for now
        LaurelEye::GlfwPlatform glfwP = LaurelEye::GlfwPlatform();
        glfwP.initialize();

        windowManager = std::make_unique<WindowManager>();
        auto window = windowManager->createWindow(LaurelEye::WindowDescription());

        inputManager = std::make_unique<InputManager>(*window);
        memoryManager = std::make_unique<MemoryManager>();
        assetManager = std::make_unique<IO::AssetManager>();
        eventManager = std::make_unique<EventManager>();
       
        ctx.registerService<InputManager>(inputManager.get());
        ctx.registerService<WindowManager>(windowManager.get());
        ctx.registerService<MemoryManager>(memoryManager.get());
        ctx.registerService<IO::AssetManager>(assetManager.get());
        ctx.registerService<EventManager>(eventManager.get());
    }

    void ResourceCoordinator::update(float deltaTime) {
        inputManager->update();
    }

    void ResourceCoordinator::shutdown() {
        std::cout << "Resources Shutting Down" << std::endl;
        inputManager.reset();
        windowManager.reset();
        memoryManager.reset();
        assetManager.reset();
    }
} // namespace LaurelEye
