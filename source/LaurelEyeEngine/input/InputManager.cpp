#include "LaurelEyeEngine/input/InputManager.h"

namespace LaurelEye {
    void InputManager::update() {
        inputBackend->update();
    }

    bool InputManager::isKeyPressed(Key key) {
        return inputBackend->isKeyPressed(key);
    }
    bool InputManager::isKeyHeld(Key key) {
        return inputBackend->isKeyHeld(key);
    }
    bool InputManager::isKeyReleased(Key key) {
        return inputBackend->isKeyReleased(key);
    }

    bool InputManager::isMouseButtonPressed(MouseButton button) {
        return inputBackend->isMouseButtonPressed(button);
    }
    bool InputManager::isMouseButtonHeld(MouseButton button) {
        return inputBackend->isMouseButtonHeld(button);
    }
    bool InputManager::isMouseButtonReleased(MouseButton button) {
        return inputBackend->isMouseButtonReleased(button);
    }
    std::pair<float, float> InputManager::getMousePosition() {
        return inputBackend->getMousePosition();
    }

    bool InputManager::isButtonPressed(GamepadButton button) {
        return inputBackend->isButtonPressed(button);
    }
    bool InputManager::isButtonHeld(GamepadButton button) {
        return inputBackend->isButtonHeld(button);
    }
    bool InputManager::isButtonReleased(GamepadButton button) {
        return inputBackend->isButtonReleased(button);
    }
    float InputManager::getGamepadAxis(GamepadAxes axis) {
        return inputBackend->getGamepadAxis(axis);
    }

}
