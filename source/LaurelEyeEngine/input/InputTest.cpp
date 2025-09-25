#include "LaurelEyeEngine/input/InputSystem.h"
#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include "LaurelEyeEngine/window/WindowManager.h"


int Test() {

    LaurelEye::GlfwPlatform glfwP = LaurelEye::GlfwPlatform();
    glfwP.initialize();

    LaurelEye::WindowManager wm = LaurelEye::WindowManager();
    auto window = wm.createWindow(LaurelEye::WindowDescription());

    LaurelEyeEngine::InputSystem inputSystem((GLFWwindow*)window->getNativeHandle());

    std::cout << "Press W, A, S, D, or Escape to test input. Escape will exit.\n";

    while ( !inputSystem.isKeyPressed(LaurelEyeEngine::Key::Escape) ) {

        glfwP.update();
        inputSystem.update();

        if ( inputSystem.isKeyPressed(LaurelEyeEngine::Key::W) )
            std::cout << "W key pressed!" << std::endl;
        if ( inputSystem.isKeyPressed(LaurelEyeEngine::Key::A) )
            std::cout << "A key pressed!" << std::endl;
        if ( inputSystem.isKeyPressed(LaurelEyeEngine::Key::S) )
            std::cout << "A key pressed!" << std::endl;
        if ( inputSystem.isKeyPressed(LaurelEyeEngine::Key::D) )
            std::cout << "A key pressed!" << std::endl;

        if ( inputSystem.isKeyHeld(LaurelEyeEngine::Key::W) )
            std::cout << "W key Held!" << std::endl;
        if ( inputSystem.isKeyHeld(LaurelEyeEngine::Key::A) )
            std::cout << "A key Held!" << std::endl;
        if ( inputSystem.isKeyHeld(LaurelEyeEngine::Key::S) )
            std::cout << "A key Held!" << std::endl;
        if ( inputSystem.isKeyHeld(LaurelEyeEngine::Key::D) )
            std::cout << "A key Held!" << std::endl;

        if ( inputSystem.isKeyReleased(LaurelEyeEngine::Key::W) )
            std::cout << "W key Released!" << std::endl;
        if ( inputSystem.isKeyReleased(LaurelEyeEngine::Key::A) )
            std::cout << "A key Released!" << std::endl;
        if ( inputSystem.isKeyReleased(LaurelEyeEngine::Key::S) )
            std::cout << "A key Released!" << std::endl;
        if ( inputSystem.isKeyReleased(LaurelEyeEngine::Key::D) )
            std::cout << "A key Released!" << std::endl;

        if ( inputSystem.isMouseButtonPressed(LaurelEyeEngine::MouseButton::Left) )
            std::cout << "Left Click pressed!" << std::endl;
        if ( inputSystem.isMouseButtonPressed(LaurelEyeEngine::MouseButton::Right) )
            std::cout << "Right Click pressed!" << std::endl;

        if ( inputSystem.isMouseButtonHeld(LaurelEyeEngine::MouseButton::Left) )
            std::cout << "Left Click Held!" << std::endl;
        if ( inputSystem.isMouseButtonHeld(LaurelEyeEngine::MouseButton::Right) )
            std::cout << "Right Click Held!" << std::endl;

        if ( inputSystem.isMouseButtonReleased(LaurelEyeEngine::MouseButton::Left) )
            std::cout << "Left Click Released!" << std::endl;
        if ( inputSystem.isMouseButtonReleased(LaurelEyeEngine::MouseButton::Right) )
            std::cout << "Right Click Released!" << std::endl;

        // Sleep or wait a bit to avoid spamming the console
        glfwWaitEventsTimeout(0.05);
    }

    wm.shutdown();
    glfwP.shutdown();
    return 0;
};
