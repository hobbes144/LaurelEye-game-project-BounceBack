/// @file   LGlfwOpenGLWindowSurface.h
/// @author Your Name (your.email@domain.com)
/// @date   10-09-2025
/// @brief  Provides an OpenGL window surface using GLFW for the engine.
///
/// This class implements the IWindowSurfaceProvider interface to allow the
/// engine to create and manage an OpenGL rendering context tied to a GLFW window.


#pragma once

#include "LaurelEyeEngine/graphics/surface/IWindowSurfaceProvider.h"
#include "LaurelEyeEngine/window/IWindow.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace LaurelEye::Graphics {

    /// @class LGlfwOpenGLWindowSurface
    /// @brief GLFW-based OpenGL window surface provider for the engine.
    ///
    /// This class wraps GLFW window creation and management for OpenGL rendering.
    /// It handles context attachment, frame lifecycle, and shutdown.
    class LGlfwOpenGLWindowSurface final : public IWindowSurfaceProvider {
    public:
        /// @brief Returns the graphics backend implemented by this surface.
        /// @return GraphicsBackend::OpenGL
        GraphicsBackend backend() const override { return GraphicsBackend::OpenGL; }

        /// @brief Attaches this OpenGL surface to a given window.
        /// @param window The engine window to attach to.
        void attachToWindow(IWindow&) override;
        /// @brief Shuts down the window surface and releases any OpenGL/GLFW resources.
        void shutdown() override;

        /// @brief Prepares the frame for rendering.
        ///
        /// This function should be called at the beginning of each render frame.
        void beginFrame() override;
        /// @brief Finalizes the frame and presents it on screen.
        ///
        /// This function should be called at the end of each render frame.
        void endFrame() override;

        
    private:
        /// @brief Width of the window surface.
        /// @note Consider swapping this to a SizeRegistry in the future.
        int width;
        /// @brief Height of the window surface.
        /// @note Consider swapping this to a SizeRegistry in the future.
        int height;

        void resizeSurface(int w, int h) override;

        void resizeSurfaceCallback() override;
        int pendingWidth = 0;
        int pendingHeight = 0;
        bool resizePending = false;
    };
}
