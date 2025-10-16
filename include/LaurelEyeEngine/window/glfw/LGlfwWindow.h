/// @file   LGlfwWindow.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief GLFW Window.

#pragma once

#include "LaurelEyeEngine/window/IWindow.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace LaurelEye {
    /// @class LGlfwWindow
    /// @brief This object handles the GLFW Window creation and callbacks.
    ///
    class LGlfwWindow final : public IWindow {
    public:
        LGlfwWindow(const WindowDescription&);
        ~LGlfwWindow() override;
        /// \copydoc IWindow::shouldClose
        bool shouldClose() const override;
        /// \copydoc IWindow::getNativeHandle
        NativeWindowHandle getNativeHandle() override;

        /// \copydoc IWindow::setTitle
        void setTitle(std::string _title) override;
        /// \copydoc IWindow::setWidth
        void setWidth(int _width) override;
        /// \copydoc IWindow::setHeight
        void setHeight(int _height) override;
        /// \copydoc IWindow::setVsync
        void setVsync(bool flag) override;
        /// \copydoc IWindow::setFullscreen
        void setFullscreen(bool flag) override;

    private:
        GLFWwindow* nativeHandle = nullptr;

        // TODO: Implement all the window callbacks here
        // Consider adding a single event callback setter?
    };
} // namespace LaurelEye
