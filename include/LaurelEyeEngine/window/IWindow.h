#pragma once

#include <string>

namespace LaurelEye {

    // TODO: Figure out how to get rid of this and keep abstraction
    // Making this template forces import of GLFW in a lot of places
    // even when not necessary.
    using PlatformWindow = void;

    enum WindowMode { Fullscreen,
                      Windowed,
                      Borderless };

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
    };

    class IWindow {
    public:
        virtual ~IWindow() noexcept = default;

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

        virtual PlatformWindow* getNativeHandle() = 0;

        // Other functions:
        // - show()
        // - hide()

        // Copied from old engine:
        virtual void setTitle(std::string _title) = 0;
        virtual void setWidth(int _width) = 0;
        virtual void setHeight(int _height) = 0;
        virtual void setVsync(bool flag) = 0;
        virtual void setFullscreen(bool flag) = 0;

        int getWidth() const { return attributes.width; }
        int getHeight() const { return attributes.height; }
        float getAspectRatio() const {
            return float(attributes.width) / float(attributes.height);
        }

    protected:
        /// Window attributes for this window
        WindowDescription attributes;
    };

} // namespace LaurelEye
