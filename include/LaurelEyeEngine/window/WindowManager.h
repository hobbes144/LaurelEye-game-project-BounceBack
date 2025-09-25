#pragma once
#include "LaurelEyeEngine/window/IWindow.h"

#include <memory>
#include <vector>

namespace LaurelEye {

    // TODO: Consider adding a resource manager abstraction for
    // WindowManager and SurfaceManager, and anything else.
    class WindowManager {
    public:
        void initialize();            // Dummy call, but may want to auto create main window?
        void update(float deltaTime); // Dummy call
        void shutdown();              // Dummy call

        IWindow* createWindow(const WindowDescription& wDesc);
        IWindow* getWindow(int id);

    private:
        std::vector<std::unique_ptr<IWindow>> managedWindows;
        int mainWindowId;
    };

} // namespace LaurelEye
