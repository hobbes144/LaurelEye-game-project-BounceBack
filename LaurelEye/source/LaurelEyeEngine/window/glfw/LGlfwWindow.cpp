/// @file   LGlfwWindow.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of GLFW Window

#include "LaurelEyeEngine/window/glfw/LGlfwWindow.h"
// We should be using Graphics.h to define valid backends, and init accordingly.
// #include "LaurelEyeEngine/graphics/Graphics.h"
#include "LaurelEyeEngine/window/IWindow.h"

#include <cassert>
#include <GLFW/glfw3.h>

namespace LaurelEye {

    LGlfwWindow::LGlfwWindow(const WindowDescription& wDesc) : IWindow() {
        this->attributes = wDesc;

        GLFWmonitor* monitor = nullptr;

        if ( this->attributes.mode == WindowMode::Fullscreen ) {
            monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            this->attributes.width = mode->width;
            this->attributes.height = mode->height;
        }
        else {
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

        // TODO: Below need to be implemented:
        // glfwSetWindowMaximizeCallback(nativeHandle, maximizedCallback);
        glfwSetFramebufferSizeCallback(nativeHandle, onResizeCallback); // Maybe this should be in the renderer?
        // glfwSetWindowFocusCallback(nativeHandle, focusCallback);
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
        // To be implemented here
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
        assert(false && "LAURELEYE::WINDOW::SETFULLSCREEN::UNIMPLEMENTED");
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
            std::cout << "Window size: (" << width << ", " << height << ")" << std::endl;
        }
    }
} // namespace LaurelEye
