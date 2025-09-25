#include "LaurelEyeEngine/input/InputSystem.h"


int Test() {
    if ( !glfwInit() ) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Input Test", nullptr, nullptr);
    if ( !window ) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    LaurelEyeEngine::InputSystem inputSystem(window);

    std::cout << "Press W, A, S, D, or Escape to test input. Escape will exit.\n";

    while (!inputSystem.isKeyPressed(LaurelEyeEngine::Key::Escape)) {
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

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
};
