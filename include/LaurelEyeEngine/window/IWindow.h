/// @file   IWindow.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief IWindow class that handles game windows.

#pragma once

#include <string>
#include <functional>

namespace LaurelEye {

    // TODO: Figure out how to get rid of this and keep abstraction
    // Making this template forces import of GLFW in a lot of places
    // even when not necessary.
    using NativeWindowHandle = void*;

    enum WindowMode { Fullscreen,
                      Windowed,
                      Borderless };

    /// @class WindowDescription
    /// @brief Window description used to create the window.
    struct WindowDescription {
        /// Window title
        std::string title = "LaurelEyeEngine Application";
        // Width of the window
        int width = 1280;
        /// Height of the window
        int height = 720;
        /// Windowed width
        int windowedWidth = 1280;
        /// Windowed height
        int windowedHeight = 720;
        /// Windowed X position
        int windowedX = 100;
        /// Windowed Y position
        int windowedY = 100;
        // float refreshRate = 60.0f; // unimplemented, but very doable
        /// Window mode (enum WindowMode)
        WindowMode mode = WindowMode::Windowed;
        bool vsync = true;
        // bool resizable = false; // unimplemented
        bool setupOpenGLContext = true;
    };

    /// @class IWindow
    /// @brief Window interface that provides interactions with the game
    /// window.
    ///
    class IWindow {
    public:
        virtual ~IWindow() noexcept = default;

        /// @brief Get if the window close request event has occurred.
        ///
        /// @return `true` if close requested.
        virtual bool shouldClose() const = 0;

        // Setters and Getters
        // We might want to have:
        // - title
        // - width
        // - height
        // - vsync
        // - shouldClose
        // - nativeWindow (get only)
        // - aspect ratio
        // - WindowMode
        // - inFocus (get only)

        /// @brief Get native GLFW window handle.
        ///
        /// @return GLFWwindow pointer.
        virtual NativeWindowHandle getNativeHandle() = 0;

        // Other functions:
        // - show()
        // - hide()

        // Copied from old engine:
        /// @brief Set the window title.
        ///
        /// @param _title Title of the window.
        virtual void setTitle(std::string _title) = 0;
        /// @brief Set the window width.
        ///
        /// @param _width Window width.
        virtual void setWidth(int _width) = 0;
        /// @brief Set the window height.
        ///
        /// @param _height Window height.
        virtual void setHeight(int _height) = 0;
        /// @brief Set window VSync.
        ///
        /// @param flag `true` if vsync should be turned on.
        virtual void setVsync(bool flag) = 0;
        /// @brief Set if window should be full screen.
        ///
        /// @param flag `true` if full screen.
        virtual void setFullscreen(bool flag) = 0;

        /// @brief Get window width.
        ///
        /// @return Width of the window.
        int getWidth() const { return attributes.width; }
        /// @brief Get window height.
        ///
        /// @return Window height.
        int getHeight() const { return attributes.height; }
        /// @brief Get aspect ratio.
        ///
        /// @return Aspect ratio of the screen.
        float getAspectRatio() const {
            return float(attributes.width) / float(attributes.height);
        }

        // TODO: For window resize callback, set up a single SizeRegistry
        // as single source of truth, and have it queried during update
        // (or use) of concerned entities. eg. FBOs will be updated via
        // RenderGraphBuilder, WSP will read it during beginFrame.
        // resizeCallback -> IWindowSurfaceProvider.resizeSurface ->
        //                   SizeRegistry.updateSwapchainSize
        std::function<void(NativeWindowHandle, int, int)> surfaceResizeCallback;
    protected:
        /// Window attributes for this window
        WindowDescription attributes;
    };

} // namespace LaurelEye
