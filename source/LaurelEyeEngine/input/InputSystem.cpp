#include "LaurelEyeEngine/input/InputSystem.h"

namespace LaurelEyeEngine {
    void InputSystem::update() {
        inputBackend->update();
    }

    bool InputSystem::isKeyPressed(Key key) {
        return inputBackend->isKeyPressed(key);
    }
    bool InputSystem::isKeyHeld(Key key) {
        return inputBackend->isKeyHeld(key);
    }
    bool InputSystem::isKeyReleased(Key key) {
        return inputBackend->isKeyReleased(key);
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) {
        return inputBackend->isMouseButtonPressed(button);
    }
    bool InputSystem::isMouseButtonHeld(MouseButton button) {
        return inputBackend->isMouseButtonHeld(button);
    }
    bool InputSystem::isMouseButtonReleased(MouseButton button) {
        return inputBackend->isMouseButtonReleased(button);
    }

    bool InputSystem::isButtonPressed(GamepadButton button) {
        return inputBackend->isButtonPressed(button);
    }
    bool InputSystem::isButtonHeld(GamepadButton button) {
        return inputBackend->isButtonHeld(button);
    }
    bool InputSystem::isButtonReleased(GamepadButton button) {
        return inputBackend->isButtonReleased(button);
    }

    std::pair<float, float> InputSystem::getMousePosition() {
        return inputBackend->getMousePosition();
    }
}
