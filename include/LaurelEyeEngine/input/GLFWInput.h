#pragma once


#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "Input.h"

namespace LaurelEye {
    class GLFWInput : public Input {
    public:
        GLFWInput(GLFWwindow* window)
            : pWindow(window) {
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

    private:
        GLFWwindow* pWindow;
        std::unordered_map<int, KeyState> keyStates;
        std::unordered_map<int, KeyState> mouseStates;
        std::unordered_map<int, KeyState> gamepadStates;
        std::pair<float, float> currMousePos;

        /*Conversion functions to turn struct values from Input.h to GLFW codes*/
        int toGLFWKey(Key key);
        int toGLFWMouse(MouseButton button);
        int toGLFWGamepad(GamepadButton button);
    };
}

