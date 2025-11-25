/****************************************************************
// File Name: ElementManagerComponent
****************************************************************/

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/graphics/graphics_components/UIComponent.h"
#include "LaurelEyeEngine/input/IInput.h"
#include "LaurelEyeEngine/UI/ButtonEvent.h"
#include "LaurelEyeEngine/UI/UILayout.h"

#include <map>
#include <stack>
#include <string>

namespace LaurelEye {
    class UIElementManager {
    public:
        UIElementManager()
            : currentComponent(nullptr), listener(0) {}
        ~UIElementManager();

        void setCurrentUIComponent(Graphics::UIComponent* component);

        void setInputAndWindow(LaurelEye::InputManager* inputMgr, LaurelEye::IWindow* _window);

        void update(float deltaTime);

        void registerUIElement(Graphics::UIComponent* elementToRegister);

        void deregisterUIElement(Graphics::UIComponent* elementToDeregister);

        void removeAllUIElements();

        void clearInputMaps();

        void pushInputMap(const std::string& inputMapName);

        void popInputMap();

        void registerInputMap(UILayout* inputMap);

        void deregisterInputMap(UILayout* inputMap);

        void setInputManager(LaurelEye::InputManager* inputMgr) {
            inputManager = inputMgr;
        }

        void setUIWindow(LaurelEye::IWindow* _window) {
            window = _window;
        }

        void enableCurrentUI();
        void disableCurrentUI();
        void disableAllUI();

        void toLeft();
        void toRight();
        void toUp();
        void toDown();

        void setActivateButton(GamepadButton button) { activateButton = button; }
        void setEscapeButton(GamepadButton button) { escapeButton = button; }
        void setMouseActivateButton(MouseButton button) { mouseActivateButton = button; }
        void setActivateKey(Key key) { activateKey = key; }
        void setUpKey(Key key) { upKey = key; }
        void setDownKey(Key key) { downKey = key; }
        void setLeftKey(Key key) { leftKey = key; }
        void setRightKey(Key key) { rightKey = key; }
        void setEscapeKey(Key key) { escapeKey = key; }
        void setIsUIActive(bool isActive);
        void updateIsUIActive();
        bool getIsUIActive() const { return isUIActive; }

        void dispatchActivateEvent();

        void registerListener();

        void deregisterListener();

        void setEngineContext(EngineContext& newContext) {
            context = &newContext;
        }

        // Converts mouse position to UI coordinate system (-1 to 1)
        std::pair<float, float> mouseToUICoordinate();
        // Converts UI coordinate system (-1 to 1) to mouse position
        std::pair<float, float> uiToMouseCoordinate();

    private:
        float hoverDelay = 0.0f;
        const float hoverCooldown = 0.1f;

        bool isUIActive = true;

        Graphics::UIComponent* currentComponent = nullptr;

        std::unordered_map<std::string, Graphics::UIComponent*> elements = std::unordered_map<std::string, Graphics::UIComponent*>(); // All available UI elements

        std::stack<UILayout*> inputMaps = std::stack<UILayout*>();
        std::unordered_map<std::string, UILayout*> inputMapStorage = std::unordered_map<std::string, UILayout*>();

        MouseButton mouseActivateButton = MouseButton::Left;
        Key activateKey = Key::Space;
        Key upKey = Key::arrowUp;
        Key downKey = Key::arrowDown;
        Key leftKey = Key::arrowLeft;
        Key rightKey = Key::arrowRight;
        Key escapeKey = Key::M;

        GamepadButton activateButton = GamepadButton::gamepadA;
        GamepadButton escapeButton = GamepadButton::gamepadB;

        uint32_t listener;

        LaurelEye::InputManager* inputManager = nullptr;
        LaurelEye::IWindow* window = nullptr;
        EngineContext* context = nullptr;
    };
} // namespace LaurelEye
