#pragma once

#include "GLFWInput.h"
#include <memory>

namespace LaurelEye {
    class InputSystem {
    public:
        InputSystem(GLFWwindow* window)
            : inputBackend(std::make_unique<GLFWInput>(window)) {}
        void update();

        /*Keyboard functions*/
        bool isKeyPressed(Key key);
        bool isKeyHeld(Key key);
        bool isKeyReleased(Key key);

        /*Mouse functions*/
        bool isMouseButtonPressed(MouseButton button);
        bool isMouseButtonHeld(MouseButton button);
        bool isMouseButtonReleased(MouseButton button);
        std::pair<float, float> getMousePosition();

        /*Gamepad functions*/
        bool isButtonPressed(GamepadButton button);
        bool isButtonHeld(GamepadButton button);
        bool isButtonReleased(GamepadButton button);

    private:
        std::unique_ptr<Input> inputBackend;
    };
} // namespace LaurelEyeEngine
