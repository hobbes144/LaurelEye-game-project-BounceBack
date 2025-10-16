/// @file   WindowManager.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Window manager class.

#pragma once

#include "LaurelEyeEngine/window/IWindow.h"

#include <memory>
#include <vector>

namespace LaurelEye {

    enum class WindowBackend {
        GLFW
    };

    // TODO: Consider adding a resource manager abstraction for
    // WindowManager and SurfaceManager, and anything else.
    class WindowManager {
    public:
        WindowManager(WindowBackend type = WindowBackend::GLFW);

        /// @brief Shutdown all the windows.
        void shutdown();              // Dummy call

        /// @brief Create a window.
        ///
        /// @param wDesc WindowDescription struct.
        /// @return IWindow pointer.
        IWindow* createWindow(const WindowDescription& wDesc);
        // Discussion: How should we handle get? Do we use IDs?

        /// @brief Get window pointer from ID.
        ///
        /// @param id ID of the window.
        /// @return IWindow pointer.
        IWindow* getWindow(int id);

    private:
        WindowBackend systemType;
        std::vector<std::unique_ptr<IWindow>> managedWindows;
        int mainWindowId = -1;
    };

} // namespace LaurelEye
