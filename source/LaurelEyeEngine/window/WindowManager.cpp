#include "LaurelEyeEngine/window/WindowManager.h"
#include "LaurelEyeEngine/window/IWindow.h"

#ifdef PLATFORM_GLFW
#include "LaurelEyeEngine/window/glfw/LGlfwWindow.h"
#else
#error "No window backend selected (define PLATFORM_GLFW or PLATFORM_SDL)"
#endif

#include <memory>
#include <vector>

namespace LaurelEye {

    void WindowManager::initialize() {}

    void WindowManager::update(float deltaTime) {
        // std::vector<std::unique_ptr<IWindow>> pendingCloseWindows;
        // managedWindows.erase(
        //     std::remove_if(
        //         managedWindows.begin(),
        //         managedWindows.end(),
        //         [&](std::unique_ptr<IWindow>& window) { return window->shouldClose(); }),
        //     managedWindows.end());
    }

    void WindowManager::shutdown() {
        managedWindows.clear();
    }

    IWindow* WindowManager::createWindow(const WindowDescription& wDesc) {
#ifdef PLATFORM_GLFW
        auto pWindow = std::make_unique<LaurelEye::LGlfwWindow>(wDesc);
#endif
        IWindow* rawPWindow = pWindow.get();
        managedWindows.emplace_back(std::move(pWindow));
        return rawPWindow;
    }

    IWindow* WindowManager::getWindow(int id) {
        if ( id < managedWindows.size() - 1 ) return managedWindows[id].get();
        else return nullptr;
    }

} // namespace LaurelEye
