#pragma once

#include "LaurelEyeEngine/window/IWindow.h"

#include <GLFW/glfw3.h>

namespace LaurelEye {
    class LGlfwWindow final : public IWindow {
    public:
        LGlfwWindow(const WindowDescription&);
        ~LGlfwWindow() override;
        bool shouldClose() const override;
        PlatformWindow* getNativeHandle() override;

        void setTitle(std::string _title) override;
        void setWidth(int _width) override;
        void setHeight(int _height) override;
        void setVsync(bool flag) override;
        void setFullscreen(bool flag) override;

    private:
        GLFWwindow* nativeHandle = nullptr;

        // TODO: Implement all the window callbacks here
        // Consider adding a single event callback setter?
    };
} // namespace LaurelEye
