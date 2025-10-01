
/// @file    GLFWInput.h
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com)
/// @par     **Digipen Email**
///    jacob.wilburn@digipen.edu
/// @date    9-22-25
/// @brief   File containing the implementation of the input manager for the LaurelEyeEngine
/// Uses a pointer to the backend input system to execute the logic of the functions

#pragma once

#include "GLFWInput.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include <memory>

namespace LaurelEye {
    class InputManager {
    public:
        InputManager(IWindow& window)
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
        float getGamepadAxis(GamepadAxes axis);

    private:
        std::unique_ptr<IInput> inputBackend;
    };
} // namespace LaurelEyeEngine
