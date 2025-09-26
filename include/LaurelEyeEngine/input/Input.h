#pragma once

#include <unordered_map>
#include "InputCodes.h" 
namespace LaurelEye {

    struct KeyState {
        bool isPressed = false;
        bool isHeld = false;
        bool isReleased = false;
    };

    class Input {
    public:
        virtual void update() = 0;

        /*Virtual functions for checking states for the keyboard*/
        virtual bool isKeyPressed(Key key) = 0;
        virtual bool isKeyHeld(Key key) = 0;
        virtual bool isKeyReleased(Key key) = 0;

        /*Virtual functions for checking states for the mouse*/
        virtual bool isMouseButtonPressed(MouseButton button) = 0;
        virtual bool isMouseButtonHeld(MouseButton button) = 0;
        virtual bool isMouseButtonReleased(MouseButton button) = 0;
        virtual void updateMousePosition() = 0;
        virtual std::pair<float, float> getMousePosition() = 0;

        /*Virtual functions for checking states for gamepads*/
        virtual bool isButtonPressed(GamepadButton button) = 0;
        virtual bool isButtonHeld(GamepadButton button) = 0;
        virtual bool isButtonReleased(GamepadButton button) = 0;
    };
}
