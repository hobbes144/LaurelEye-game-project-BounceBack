#include "LaurelEyeEngine/window/glfw/LGlfwWindow.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include <GLFW/glfw3.h>

namespace LaurelEye {

    LGlfwWindow::LGlfwWindow(const WindowDescription& wDesc) : IWindow() {
        this->attributes.title = wDesc.title;
        this->attributes.width = wDesc.width;
        this->attributes.height = wDesc.height;
        this->attributes.mode = wDesc.mode;
        this->attributes.width = wDesc.width;

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

        nativeHandle = glfwCreateWindow(
            this->attributes.width,
            this->attributes.height,
            this->attributes.title.c_str(),
            monitor,
            nullptr);

        assert(nativeHandle && "Failed to create GLFW window");

        glfwSetWindowUserPointer(nativeHandle, this);

        if ( this->attributes.mode != WindowMode::Fullscreen ) {
            glfwSetWindowPos(nativeHandle,
                             this->attributes.windowedX,
                             this->attributes.windowedY);
        }

        // TODO: Below need to be implemented:
        // glfwSetWindowMaximizeCallback(nativeHandle, maximizedCallback);
        // glfwSetFramebufferSizeCallback(nativeHandle, resizeCallback); // Maybe this should be in the renderer?
        // glfwSetWindowFocusCallback(nativeHandle, focusCallback);
    }

    LGlfwWindow::~LGlfwWindow() {
        glfwDestroyWindow(nativeHandle);
    }

    bool LGlfwWindow::shouldClose() const {
        return false;
    }

    PlatformWindow* LGlfwWindow::getNativeHandle() {
        return nativeHandle;
    }

    void LGlfwWindow::setTitle(std::string _title) {
        // To be implemented here
    }
    void LGlfwWindow::setWidth(int _width) {
    }
    void LGlfwWindow::setHeight(int _height) {
    }
    void LGlfwWindow::setVsync(bool flag) {
        if ( this->attributes.vsync != flag )
            glfwSwapInterval(int(flag));
    }
    void LGlfwWindow::setFullscreen(bool flag) {
    }
} // namespace LaurelEye
