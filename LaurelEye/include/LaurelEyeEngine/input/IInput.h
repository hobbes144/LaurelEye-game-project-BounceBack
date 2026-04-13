/// @file    IInput.h
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com)
/// @par     **Digipen Email**
///    jacob.wilburn@digipen.edu
/// @date    9-22-25
/// @brief   File containing the core input implementation for the LaurelEyeEngine
/// Abstracted input interface, holding virtual functions for all input functions needed

#pragma once

#include "InputCodes.h"
namespace LaurelEye {

    struct KeyState {
        bool isPressed = false;
        bool isHeld = false;
        bool isReleased = false;
    };

    class IInput {
    public:
        virtual ~IInput() = default;
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
        virtual std::pair<float, float> getMouseDelta() = 0;

        /*Virtual functions for checking states for gamepads*/
        virtual bool isButtonPressed(GamepadButton button) = 0;
        virtual bool isButtonHeld(GamepadButton button) = 0;
        virtual bool isButtonReleased(GamepadButton button) = 0;
        virtual float getGamepadAxis(GamepadAxes axis) = 0;
    };
} // namespace LaurelEye
