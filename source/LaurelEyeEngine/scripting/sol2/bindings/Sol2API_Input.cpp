#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Input.h"
#include <tuple>

namespace LaurelEye::Scripting {

    void Sol2API_Input::setup(sol::state& lua, EngineContext* ctx) {
        // input manager
        // TODO - determine how we get "InputManager"
        InputManager* inputManager = ctx->getService<InputManager>();
        if ( inputManager ) {
            setupInputManager(lua, inputManager);
        }
    }

    void Sol2API_Input::setupInputManager(sol::state& lua, InputManager* input) {
        // --- Key Enum ---
        lua.new_enum("Key",
                     "W", Key::W,
                     "A", Key::A,
                     "S", Key::S,
                     "D", Key::D,
                     "Space", Key::Space,
                     "Escape", Key::Escape,
                     "Enter", Key::Enter,
                     "Tab", Key::Tab,
                     "ArrowUp", Key::arrowUp,
                     "ArrowDown", Key::arrowDown,
                     "ArrowRight", Key::arrowRight,
                     "ArrowLeft", Key::arrowLeft,
                     "One", Key::One,
                     "Two", Key::Two,
                     "Three", Key::Three,
                     "Four", Key::Four,
                     "Five", Key::Five,
                     "Six", Key::Six);

        // --- MouseButton Enum ---
        lua.new_enum("MouseButton",
                     "Left", MouseButton::Left,
                     "Right", MouseButton::Right,
                     "Middle", MouseButton::Middle);

        // --- GamepadButton Enum ---
        lua.new_enum("GamepadButton",
                     "A", GamepadButton::gamepadA,
                     "B", GamepadButton::gamepadB,
                     "X", GamepadButton::gamepadX,
                     "Y", GamepadButton::gamepadY,
                     "LB", GamepadButton::gamepadLB,
                     "RB", GamepadButton::gamepadRB,
                     "Up", GamepadButton::gamepadUp,
                     "Down", GamepadButton::gamepadDown,
                     "Right", GamepadButton::gamepadRight,
                     "Left", GamepadButton::gamepadLeft,
                     "Start", GamepadButton::gamepadStart);

        // --- GamepadAxes Enum ---
        lua.new_enum("GamepadAxes",
                     "LStickX", GamepadAxes::gamepadLStickX,
                     "LStickY", GamepadAxes::gamepadLStickY,
                     "RStickX", GamepadAxes::gamepadRStickX,
                     "RStickY", GamepadAxes::gamepadRStickY,
                     "LT", GamepadAxes::gamepadLT,
                     "RT", GamepadAxes::gamepadRT);

        // Bind InputManager class to Lua
        lua.new_usertype<InputManager>(
            "InputManager",
            sol::no_constructor, // engine owns instance
            "isKeyPressed", &InputManager::isKeyPressed,
            "isKeyHeld", &InputManager::isKeyHeld,
            "isKeyReleased", &InputManager::isKeyReleased,

            "isMouseButtonPressed", &InputManager::isMouseButtonPressed,
            "isMouseButtonHeld", &InputManager::isMouseButtonHeld,
            "isMouseButtonReleased", &InputManager::isMouseButtonReleased,
            "getMousePosition", &InputManager::getMousePosition,

            "isButtonPressed", &InputManager::isButtonPressed,
            "isButtonHeld", &InputManager::isButtonHeld,
            "isButtonReleased", &InputManager::isButtonReleased,
            "getGamepadAxis", &InputManager::getGamepadAxis);

        // Expose the instance globally as 'Input'
        lua["Input"] = input;
    }

} // namespace LaurelEye::Scripting
