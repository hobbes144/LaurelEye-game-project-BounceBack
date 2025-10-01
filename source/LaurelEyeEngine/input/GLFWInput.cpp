#include "LaurelEyeEngine/input/GLFWInput.h"

namespace LaurelEye {
    void GLFWInput::update() {
        updateMousePosition();
        for ( auto& [key, state] : keyStates ) {
            if ( glfwGetKey(pWindow, key) == GLFW_PRESS ) {
                if ( !state.isHeld ) {
                    state.isPressed = true;
                    state.isHeld = true;
                    state.isReleased = false;
                }
                else {
                    state.isPressed = false;
                    state.isHeld = true;
                    state.isReleased = false;
                }
            }
            else {
                if ( state.isHeld ) {
                    state.isPressed = false;
                    state.isHeld = false;
                    state.isReleased = true;
                }
                else {
                    state.isPressed = false;
                    state.isHeld = false;
                    state.isReleased = false;
                }
            }
        }
        for ( auto& [button, state] : mouseStates ) {
            if ( glfwGetMouseButton(pWindow, button) == GLFW_PRESS ) {
                if ( !state.isHeld ) {
                    state.isPressed = true;
                    state.isHeld = true;
                    state.isReleased = false;
                }
                else {
                    state.isPressed = false;
                    state.isHeld = true;
                    state.isReleased = false;
                }
            }
            else {
                if ( state.isHeld ) {
                    state.isPressed = false;
                    state.isHeld = false;
                    state.isReleased = true;
                }
                else {
                    state.isPressed = false;
                    state.isHeld = false;
                    state.isReleased = false;
                }
            }
        }
        
        if ( glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1) ) {
            GLFWgamepadstate* state = new GLFWgamepadstate;
            if ( glfwGetGamepadState(GLFW_JOYSTICK_1, state) ) {
                for ( int i = 0; i < 15; i++ ) {
                    if ( gamepadStates.find(i) == gamepadStates.end() ) {
                        gamepadStates[i] = KeyState();
                    }
                    else {
                        if ( state->buttons[i] == GLFW_PRESS ) {
                            if ( !gamepadStates[i].isHeld ) {
                                gamepadStates[i].isPressed = true;
                                gamepadStates[i].isHeld = true;
                                gamepadStates[i].isReleased = false;
                            }
                            else {
                                gamepadStates[i].isPressed = false;
                                gamepadStates[i].isHeld = true;
                                gamepadStates[i].isReleased = false;
                            }
                        }
                        else {
                            if ( gamepadStates[i].isHeld ) {
                                gamepadStates[i].isPressed = false;
                                gamepadStates[i].isHeld = false;
                                gamepadStates[i].isReleased = true;
                            }
                            else {
                                gamepadStates[i].isPressed = false;
                                gamepadStates[i].isHeld = false;
                                gamepadStates[i].isReleased = false;
                            }
                        }
                    }
                }
                for ( int j = 0; j < 6; j++ ) {
                    axisStates[j] = state->axes[j];
                }
            }
        }
    }

    bool GLFWInput::isKeyPressed(Key key) {
        int glfwKey = toGLFWKey(key);
        return keyStates[static_cast<int>(glfwKey)].isPressed;
    }
    bool GLFWInput::isKeyHeld(Key key) {
        int glfwKey = toGLFWKey(key);
        return keyStates[static_cast<int>(glfwKey)].isHeld;
    }
    bool GLFWInput::isKeyReleased(Key key) {
        int glfwKey = toGLFWKey(key);
        return keyStates[static_cast<int>(glfwKey)].isReleased;
    }

    bool GLFWInput::isMouseButtonPressed(MouseButton button) {
        int glfwButton = toGLFWMouse(button);
        return mouseStates[static_cast<int>(glfwButton)].isPressed;
    }
    bool GLFWInput::isMouseButtonHeld(MouseButton button) {
        int glfwButton = toGLFWMouse(button);
        return mouseStates[static_cast<int>(glfwButton)].isHeld;
    }
    bool GLFWInput::isMouseButtonReleased(MouseButton button) {
        int glfwButton = toGLFWMouse(button);
        return mouseStates[static_cast<int>(glfwButton)].isReleased;
    }

    void GLFWInput::updateMousePosition() {
        double x, y;
        glfwGetCursorPos(pWindow, &x, &y);
        currMousePos.first = static_cast<float>(x);
        currMousePos.second = static_cast<float>(y);
    }

    std::pair<float, float> GLFWInput::getMousePosition() {
        return currMousePos;
    }

    bool GLFWInput::isButtonPressed(GamepadButton button) {
        if ( !glfwJoystickPresent(GLFW_JOYSTICK_1) ) {
            return false;
        }
        int glfwButton = toGLFWGamepadButton(button);
        return gamepadStates[static_cast<int>(glfwButton)].isPressed;
    }
    bool GLFWInput::isButtonHeld(GamepadButton button) {
        if ( !glfwJoystickPresent(GLFW_JOYSTICK_1) ) {
            return false;
        }
        int glfwButton = toGLFWGamepadButton(button);
        return gamepadStates[static_cast<int>(glfwButton)].isHeld;
    }
    bool GLFWInput::isButtonReleased(GamepadButton button) {
        if ( !glfwJoystickPresent(GLFW_JOYSTICK_1) ) {
            return false;
        }
        int glfwButton = toGLFWGamepadButton(button);
        return gamepadStates[static_cast<int>(glfwButton)].isReleased;
    }

    float GLFWInput::getGamepadAxis(GamepadAxes axis) {
        if ( !glfwJoystickPresent(GLFW_JOYSTICK_1) ) {
            return false;
        }
        int glfwAxis = toGLFWGamepadAxis(axis);
        return axisStates[static_cast<int>(glfwAxis)];
    }

    int GLFWInput::toGLFWKey(Key key) {
        switch ( key ) {
        case Key::W:
            return GLFW_KEY_W;
        case Key::A:
            return GLFW_KEY_A;
        case Key::S:
            return GLFW_KEY_S;
        case Key::D:
            return GLFW_KEY_D;
        case Key::Space:
            return GLFW_KEY_SPACE;
        case Key::Escape:
            return GLFW_KEY_ESCAPE;
        case Key::arrowDown:
            return GLFW_KEY_DOWN;
        case Key::arrowUp:
            return GLFW_KEY_UP;
        case Key::arrowLeft:
            return GLFW_KEY_LEFT;
        case Key::arrowRight:
            return GLFW_KEY_RIGHT;
        case Key::Tab:
            return GLFW_KEY_TAB;
        default:
            return -1;
        }
    }

    int GLFWInput::toGLFWMouse(MouseButton button) {
        switch ( button ) {
        case MouseButton::Left:
            return GLFW_MOUSE_BUTTON_LEFT;
        case MouseButton::Right:
            return GLFW_MOUSE_BUTTON_RIGHT;
        case MouseButton::Middle:
            return GLFW_MOUSE_BUTTON_MIDDLE;
        default:
            return -1;
        }
    }

    int GLFWInput::toGLFWGamepadButton(GamepadButton button) {
        switch ( button ) {
        case GamepadButton::gamepadA:
            return GLFW_GAMEPAD_BUTTON_A;
        case GamepadButton::gamepadB:
            return GLFW_GAMEPAD_BUTTON_B;
        case GamepadButton::gamepadX:
            return GLFW_GAMEPAD_BUTTON_X;
        case GamepadButton::gamepadY:
            return GLFW_GAMEPAD_BUTTON_Y;
        case GamepadButton::gamepadLB:
            return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
        case GamepadButton::gamepadRB:
            return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
        case GamepadButton::gamepadUp:
            return GLFW_GAMEPAD_BUTTON_DPAD_UP;
        case GamepadButton::gamepadDown:
            return GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
        case GamepadButton::gamepadLeft:
            return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
        case GamepadButton::gamepadRight:
            return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
        case GamepadButton::gamepadStart:
            return GLFW_GAMEPAD_BUTTON_START;
        default:
            return -1;
        }
    }

    int GLFWInput::toGLFWGamepadAxis(GamepadAxes axis) {
        switch (axis){
        case GamepadAxes::gamepadLStickX:
            return GLFW_GAMEPAD_AXIS_LEFT_X;
        case GamepadAxes::gamepadLStickY:
            return GLFW_GAMEPAD_AXIS_LEFT_Y;
        case GamepadAxes::gamepadRStickX:
            return GLFW_GAMEPAD_AXIS_RIGHT_X;
        case GamepadAxes::gamepadRStickY:
            return GLFW_GAMEPAD_AXIS_RIGHT_Y;
        case GamepadAxes::gamepadLT:
            return GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
        case GamepadAxes::gamepadRT:
            return GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
        default:
            return -1;
        }
    }
}
