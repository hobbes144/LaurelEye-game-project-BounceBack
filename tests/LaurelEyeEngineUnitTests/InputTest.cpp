#include "LaurelEyeEngine/input/InputManager.h"
#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include "LaurelEyeEngine/window/WindowManager.h"

namespace LaurelEye {
    int inputTest() {

        LaurelEye::GlfwPlatform glfwP = LaurelEye::GlfwPlatform();
        glfwP.initialize();

        LaurelEye::WindowManager wm = LaurelEye::WindowManager();
        auto window = wm.createWindow(LaurelEye::WindowDescription());

        LaurelEye::InputManager inputManager(*window);

        std::cout << "Press W, A, S, D, or Escape to test input. Escape will exit.\n";

        while ( !inputManager.isKeyPressed(LaurelEye::Key::Escape) ) {

            glfwP.update();
            inputManager.update();

            /*Keyboard Tests*/
            if ( inputManager.isKeyPressed(LaurelEye::Key::W) )
                std::cout << "W key pressed!" << std::endl;
            if ( inputManager.isKeyPressed(LaurelEye::Key::A) )
                std::cout << "A key pressed!" << std::endl;
            if ( inputManager.isKeyPressed(LaurelEye::Key::S) )
                std::cout << "A key pressed!" << std::endl;
            if ( inputManager.isKeyPressed(LaurelEye::Key::D) )
                std::cout << "A key pressed!" << std::endl;

            if ( inputManager.isKeyHeld(LaurelEye::Key::W) )
                std::cout << "W key Held!" << std::endl;
            if ( inputManager.isKeyHeld(LaurelEye::Key::A) )
                std::cout << "A key Held!" << std::endl;
            if ( inputManager.isKeyHeld(LaurelEye::Key::S) )
                std::cout << "A key Held!" << std::endl;
            if ( inputManager.isKeyHeld(LaurelEye::Key::D) )
                std::cout << "A key Held!" << std::endl;

            if ( inputManager.isKeyReleased(LaurelEye::Key::W) )
                std::cout << "W key Released!" << std::endl;
            if ( inputManager.isKeyReleased(LaurelEye::Key::A) )
                std::cout << "A key Released!" << std::endl;
            if ( inputManager.isKeyReleased(LaurelEye::Key::S) )
                std::cout << "A key Released!" << std::endl;
            if ( inputManager.isKeyReleased(LaurelEye::Key::D) )
                std::cout << "A key Released!" << std::endl;

            /*Mouse Tests*/
            if ( inputManager.isMouseButtonPressed(LaurelEye::MouseButton::Left) )
                std::cout << "Left Click pressed!" << std::endl;
            if ( inputManager.isMouseButtonPressed(LaurelEye::MouseButton::Right) )
                std::cout << "Right Click pressed!" << std::endl;

            if ( inputManager.isMouseButtonHeld(LaurelEye::MouseButton::Left) )
                std::cout << "Left Click Held!" << std::endl;
            if ( inputManager.isMouseButtonHeld(LaurelEye::MouseButton::Right) )
                std::cout << "Right Click Held!" << std::endl;

            if ( inputManager.isMouseButtonReleased(LaurelEye::MouseButton::Left) )
                std::cout << "Left Click Released!" << std::endl;
            if ( inputManager.isMouseButtonReleased(LaurelEye::MouseButton::Right) )
                std::cout << "Right Click Released!" << std::endl;

            std::cout << "Mouse Pos X: " << inputManager.getMousePosition().first << " Mouse Pos Y: " << inputManager.getMousePosition().second << std::endl;

            /*Gamepad Button Tests*/
            if ( inputManager.isButtonPressed(LaurelEye::GamepadButton::gamepadA) )
                std::cout << "A Button pressed" << std::endl;

            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadA) ) {
                std::cout << "A pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadB) ) {
                std::cout << "B pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadX) ) {
                std::cout << "X pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadY) ) {
                std::cout << "Y pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadLB) ) {
                std::cout << "LB pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadRB) ) {
                std::cout << "RB pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadDown) ) {
                std::cout << "Down pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadLeft) ) {
                std::cout << "Left pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadRight) ) {
                std::cout << "Right pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadUp) ) {
                std::cout << "Up pressed" << std::endl;
            }
            if ( inputManager.isButtonReleased(LaurelEye::GamepadButton::gamepadStart) ) {
                std::cout << "Start pressed" << std::endl;
            }

            /*Gamepad Axis Tests*/
            if ( inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLT) > 0.3 ) {
                std::cout << "LT Value: " << inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLT) << std::endl;
            }
            if ( inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRT) > 0.3 ) {
                std::cout << "RT Value: " << inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRT) << std::endl;
            }
            if ( inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLStickX) > 0.3 || inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLStickX) < 0.3 ) {
                std::cout << "LStick X Value: " << inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLStickX) << std::endl;
            }
            if ( inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLStickY) > 0.3 || inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLStickY) < 0.3 ) {
                std::cout << "LStick Y Value: " << inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadLStickY) << std::endl;
            }
            if ( inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRStickX) > 0.3 || inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRStickX) < 0.3 ) {
                std::cout << "RStick X Value: " << inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRStickX) << std::endl;
            }
            if ( inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRStickY) > 0.3 || inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRStickY) < 0.3 ) {
                std::cout << "RStick Y Value: " << inputManager.getGamepadAxis(LaurelEye::GamepadAxes::gamepadRStickY) << std::endl;
            }

            // Sleep or wait a bit to avoid spamming the console
            glfwWaitEventsTimeout(0.05);
        }

        wm.shutdown();
        glfwP.shutdown();
        return 0;
    };

}
