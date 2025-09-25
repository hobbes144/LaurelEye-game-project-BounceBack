#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include "LaurelEyeEngine/window/WindowManager.h"

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void windowTest() {
    LaurelEye::GlfwPlatform glfwP = LaurelEye::GlfwPlatform();
    glfwP.initialize();

    LaurelEye::WindowManager wm = LaurelEye::WindowManager();
    auto window = wm.createWindow(LaurelEye::WindowDescription());

    std::cout << "Created window" << std::endl;

    if ( wm.getWindow(0) )
        std::cout << "Window managed" << std::endl;

    for ( int i = 0; i < 120; ++i ) {
#ifdef _WIN32
        Sleep(1000.0f);
#else
        sleep(1.f);
#endif
    }

    wm.shutdown();
    glfwP.shutdown();
}
