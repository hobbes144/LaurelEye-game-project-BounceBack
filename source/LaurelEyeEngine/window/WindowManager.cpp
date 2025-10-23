/// @file   WindowManager.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of WindowManager

#include "LaurelEyeEngine/window/WindowManager.h"
#include "LaurelEyeEngine/window/IWindow.h"

#include "LaurelEyeEngine/window/glfw/LGlfwWindow.h"

#include <memory>
#include <vector>

namespace LaurelEye {

    WindowManager::WindowManager(WindowBackend type) {
        systemType = type;
    }


    // void WindowManager::update(float deltaTime) {
    //     // std::vector<std::unique_ptr<IWindow>> pendingCloseWindows;
    //     // managedWindows.erase(
    //     //     std::remove_if(
    //     //         managedWindows.begin(),
    //     //         managedWindows.end(),
    //     //         [&](std::unique_ptr<IWindow>& window) { return window->shouldClose(); }),
    //     //     managedWindows.end());
    // }

    void WindowManager::shutdown() {
        managedWindows.clear();
    }

    IWindow* WindowManager::createWindow(const WindowDescription& wDesc) {
        std::unique_ptr<IWindow> pWindow;
        switch ( systemType ) {
        case WindowBackend::GLFW:
            pWindow = std::make_unique<LaurelEye::LGlfwWindow>(wDesc);

            if (mainWindowId == -1) {
                glfwMakeContextCurrent((GLFWwindow*)pWindow->getNativeHandle());
                mainWindowId = 0;
            }
            break;
        }
        IWindow* rawPWindow = pWindow.get();
        managedWindows.emplace_back(std::move(pWindow));
        return rawPWindow;
    }

    IWindow* WindowManager::getWindow(int id) {
        if ( id < managedWindows.size() ) return managedWindows[id].get();
        else return nullptr;
    }

} // namespace LaurelEye
