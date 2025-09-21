#pragma once

#include <functional>
#include <string>

#include "GLFW/glfw3.h"

class Window {
public:
    /* System functions */
    void initialize();
    void shutdown();

    /* Pre-initialization functions */
    Window* setTitle(std::string _title);
    Window* setWidth(int _width);
    Window* setHeight(int _height);

    /* Post-initialization functions */
    Window* setVsync(bool flag);

    /* Utility functions */
    int getWidth();
    int getHeight();
    float getAspectRatio();
    bool isFullscreen = false;
    bool getShouldClose() const;
    void setShouldClose() const;
    GLFWwindow* getNativeWindow() const;
    Window* setInitialFullscreen(bool flag);
    Window* setFullscreen(bool flag);

private:
    /// Width of the window
    int width;
    /// Height of the window
    int height;
    /// Windowed width
    int windowedWidth = 1280;
    /// Windowed height
    int windowedHeight = 720;
    /// Windowed X position
    int windowedX = 100;
    /// Windowed Y position
    int windowedY = 100;

    /// Window title
    std::string title;
    /// GLFW Window object
    GLFWwindow* pWindow;
    /// Resize callback function pointer
    std::function<void(GLFWwindow*, int, int)> resizeCallback;
    /// Cursor position change callback function pointer
    std::function<void(GLFWwindow*, double, double)> cursorPosCallback;
    /// Mouse button press callback function pointer
    std::function<void(GLFWwindow*, int, int, int)> mouseButtonCallback;

    static void resizeCallbackWrapper(
        GLFWwindow* pWindow, int width, int height);
    static void cursorPosCallbackWrapper(
        GLFWwindow* pWindow, double xpos, double ypos);
    static void mouseButtonCallbackWrapper(
        GLFWwindow* pWindow, int button, int action, int mods);

    /* Renderer functions */
    void setWindowHints(const std::function<void()>& hintSetter) const;
    void setResizeCallback(
        std::function<void(GLFWwindow*, int, int)> callback);
    void setCursorPosCallback(
        std::function<void(GLFWwindow*, double, double)> callback);
    void setMouseButtonCallback(
        std::function<void(GLFWwindow*, int, int, int)> callback);
};
