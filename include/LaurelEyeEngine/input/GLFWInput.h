/// @file    GLFWInput.h
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com)
/// @par     **Digipen Email**
///    jacob.wilburn@digipen.edu
/// @date    9-22-25
/// @brief   File containing the glfw input implementation for the LaurelEyeEngine
/// Inherits from the IInput class using glfw-specific logic.

#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "IInput.h"
#include "LaurelEyeEngine/window/IWindow.h"

namespace LaurelEye {
    class GLFWInput : public IInput {
    public:
        GLFWInput(IWindow& window)
            : pWindow((GLFWwindow*)(window.getNativeHandle())) {
        }
        void update() override;

        /*Keyboard functions*/
        bool isKeyPressed(Key key) override;
        bool isKeyHeld(Key key) override;
        bool isKeyReleased(Key key) override;

        /*Mouse functions*/
        bool isMouseButtonPressed(MouseButton button) override;
        bool isMouseButtonHeld(MouseButton button) override;
        bool isMouseButtonReleased(MouseButton button) override;
        void updateMousePosition() override;
        std::pair<float, float> getMousePosition() override;

        /*Gamepad functions*/
        bool isButtonPressed(GamepadButton button) override;
        bool isButtonHeld(GamepadButton button) override;
        bool isButtonReleased(GamepadButton button) override;
        float getGamepadAxis(GamepadAxes axis) override;


    private:
        GLFWwindow* pWindow;
        std::unordered_map<int, KeyState> keyStates;
        std::unordered_map<int, KeyState> mouseStates;
        std::unordered_map<int, KeyState> gamepadStates;
        std::unordered_map<int, float> axisStates;
        std::pair<float, float> currMousePos;

        /*Conversion functions to turn struct values from Input.h to GLFW codes*/
        int toGLFWKey(Key key);
        int toGLFWMouse(MouseButton button);
        int toGLFWGamepadButton(GamepadButton button);
        int toGLFWGamepadAxis(GamepadAxes axis);
    };
}

