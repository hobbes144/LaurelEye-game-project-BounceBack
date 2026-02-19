/// @file   LGlfwWindow.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of GLFW Window

#include "LaurelEyeEngine/window/glfw/LGlfwWindow.h"
// We should be using Graphics.h to define valid backends, and init accordingly.
// #include "LaurelEyeEngine/graphics/Graphics.h"
#include "LaurelEyeEngine/logging/EngineLog.h"
#include "LaurelEyeEngine/window/IWindow.h"

#include <cassert>
#include <GLFW/glfw3.h>

namespace LaurelEye {

    LGlfwWindow::LGlfwWindow(const WindowDescription& wDesc) : IWindow() {
        this->attributes = wDesc;

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        if ( this->attributes.mode == WindowMode::Fullscreen ) {
            this->attributes.width = mode->width;
            this->attributes.height = mode->height;
        }
        else {
            monitor = nullptr;
            this->attributes.width = this->attributes.windowedWidth;
            this->attributes.height = this->attributes.windowedHeight;
        }

        if ( this->attributes.setupOpenGLContext ) {
            // TODO: The window config needs to specify the GraphicsBackend.
            // Also a lot of window params are rendering specific.
            glfwWindowHint(GLFW_SAMPLES, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            glfwWindowHint(GLFW_DEPTH_BITS, 24);
            // glfwWindowHint(GLFW_DEPTH_BITS, 32);

            // Enabling debug output
#if !defined(NDEBUG)
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        }

        nativeHandle = glfwCreateWindow(
            this->attributes.width,
            this->attributes.height,
            this->attributes.title.c_str(),
            monitor,
            nullptr);

        glfwSetInputMode(static_cast<GLFWwindow*>(nativeHandle), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Enable raw mouse motion if the OS supports it
        if ( glfwRawMouseMotionSupported() ) {
            glfwSetInputMode(static_cast<GLFWwindow*>(nativeHandle), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        assert(nativeHandle && "Failed to create GLFW window");

        glfwSetWindowUserPointer(nativeHandle, this);

        if ( this->attributes.mode != WindowMode::Fullscreen ) {
            glfwSetWindowPos(nativeHandle,
                             this->attributes.windowedX,
                             this->attributes.windowedY);
        }

        glfwSetWindowMaximizeCallback(nativeHandle, onMaximizeCallback);
        glfwSetFramebufferSizeCallback(nativeHandle, onResizeCallback);
        glfwSetWindowFocusCallback(nativeHandle, onFocusCallback);
    }

    LGlfwWindow::~LGlfwWindow() {
        glfwDestroyWindow(nativeHandle);
    }

    bool LGlfwWindow::shouldClose() const {
        return glfwWindowShouldClose(static_cast<GLFWwindow*>(nativeHandle));
    }

    NativeWindowHandle LGlfwWindow::getNativeHandle() {
        return nativeHandle;
    }

    double LGlfwWindow::getTime() {
        return glfwGetTime();
    }

    void LGlfwWindow::setTitle(std::string _title) {
        this->attributes.title = _title;
        glfwSetWindowTitle(nativeHandle, "My Window");
    }

    void LGlfwWindow::setWidth(int _width) {
        attributes.width = _width;
    }

    void LGlfwWindow::setHeight(int _height) {
        attributes.height = _height;
    }

    void LGlfwWindow::setVsync(bool flag) {
        if ( this->attributes.vsync != flag )
            glfwSwapInterval(int(flag));
    }

    void LGlfwWindow::setFullscreen(bool flag) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if ( flag ) {

            this->attributes.mode = WindowMode::Fullscreen;
            this->attributes.width = mode->width;
            this->attributes.height = mode->height;

            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

            glfwSetWindowMonitor(
                nativeHandle,
                monitor,
                0, 0,
                mode->width, mode->height,
                mode->refreshRate);
        }
        else {
            this->attributes.width = mode->width;
            this->attributes.height = mode->height;

            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

            glfwSetWindowMonitor(
                nativeHandle,
                nullptr,
                this->attributes.windowedX,
                this->attributes.windowedY,
                this->attributes.windowedWidth,
                this->attributes.windowedHeight,
                mode->refreshRate);
        }
    }

    void LGlfwWindow::setCursorMode(CursorMode mode) {
        switch ( mode ) {
        case CursorMode::Normal:
            glfwSetInputMode(nativeHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case CursorMode::Hidden:
            glfwSetInputMode(nativeHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case CursorMode::Disabled:
            glfwSetInputMode(nativeHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        case CursorMode::Captured:
            glfwSetInputMode(nativeHandle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
            break;
        }
    }

    void LGlfwWindow::onResizeCallback(GLFWwindow* window, int width, int height) {
        auto* pWindow = static_cast<LGlfwWindow*>(glfwGetWindowUserPointer(window));
        if ( pWindow ) {
            // update logical attributes (these are pixels from framebuffer callback)
            pWindow->setWidth(width);
            pWindow->setHeight(height);

            // call engine-provided resize callback if set
            if ( pWindow->surfaceResizeCallback ) {
                pWindow->surfaceResizeCallback(pWindow->getNativeHandle(), width, height);
            }
        }
        LE_DEBUG_INFO("WindowManager", "Window resized: (" << width << ", " << height << ")");
    }

    void LGlfwWindow::onMaximizeCallback(GLFWwindow* window, int maximized) {
        auto* pWindow = static_cast<LGlfwWindow*>(glfwGetWindowUserPointer(window));
        if ( pWindow ) {
            if ( pWindow->maximizeCallback ) {
                pWindow->maximizeCallback(pWindow->getNativeHandle(), maximized);
            }
        }
        LE_DEBUG_INFO("WindowManager", "Window maximized state: " << maximized);
    }

    void LGlfwWindow::onFocusCallback(GLFWwindow* window, int focused) {
        auto* pWindow = static_cast<LGlfwWindow*>(glfwGetWindowUserPointer(window));
        if ( pWindow ) {
            if ( pWindow->focusCallback ) {
                pWindow->focusCallback(pWindow->getNativeHandle(), focused);
            }
        }
        LE_DEBUG_INFO("WindowManager", "Window focus state: " << focused);
    }
} // namespace LaurelEye
