#include "LaurelEyeEngine/input/GLFWInput.h"

namespace LaurelEyeEngine {
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
        //TODO: Implement updating gamepad inputs
        
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
        int glfwButton = toGLFWGamepad(button);
        return gamepadStates[static_cast<int>(glfwButton)].isPressed;
    }
    bool GLFWInput::isButtonHeld(GamepadButton button) {
        if ( !glfwJoystickPresent(GLFW_JOYSTICK_1) ) {
            return false;
        }
        int glfwButton = toGLFWGamepad(button);
        return gamepadStates[static_cast<int>(glfwButton)].isHeld;
    }
    bool GLFWInput::isButtonReleased(GamepadButton button) {
        if ( !glfwJoystickPresent(GLFW_JOYSTICK_1) ) {
            return false;
        }
        int glfwButton = toGLFWGamepad(button);
        return gamepadStates[static_cast<int>(glfwButton)].isReleased;
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

    int GLFWInput::toGLFWGamepad(GamepadButton button) {
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

}
