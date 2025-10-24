/// @file   LGlfwOpenGLWindowSurface.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of OpenGL GLFW WindowSurface

#include "LaurelEyeEngine/graphics/surface/glfw/LGlfwOpenGLWindowSurface.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

namespace LaurelEye::Graphics {

    void LGlfwOpenGLWindowSurface::attachToWindow(IWindow& window) {
        this->nativeWindow = window.getNativeHandle();
        this->width = window.getWidth();
        this->height = window.getHeight();
        this->pendingWidth = width;
        this->pendingHeight = height;
        this->resizePending = false;

        // TODO: This is thread specific, we might need to change
        // this for multithreading/multiple windows.
        glfwMakeContextCurrent((GLFWwindow*)window.getNativeHandle());
        window.surfaceResizeCallback = [this](NativeWindowHandle, int _width, int _height) {
            this->pendingWidth = _width;
            this->pendingHeight = _height;
            this->resizeSurfaceCallback();
        };
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
    }

    // WindowSurface has no shutdown for OpenGL
    void LGlfwOpenGLWindowSurface::shutdown() {}

    void LGlfwOpenGLWindowSurface::beginFrame() {
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if ( resizePending ) {
            resizeSurface(pendingWidth, pendingHeight);
        }
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void LGlfwOpenGLWindowSurface::endFrame() {
        glfwSwapBuffers((GLFWwindow*)nativeWindow);
    }

    void LGlfwOpenGLWindowSurface::resizeSurface(int _width, int _height) {
        this->width = _width;
        this->height = _height;
        glViewport(0, 0, width, height);
        resizePending = false;
    }

    void LGlfwOpenGLWindowSurface::resizeSurfaceCallback() {
        resizePending = true;
    }
}
