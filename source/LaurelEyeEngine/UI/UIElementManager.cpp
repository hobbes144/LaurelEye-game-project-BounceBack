/// @file   UIElementManager.cpp
/// @author Daoming Wang (KeitherJinn@Outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-30-2025
/// @brief Implementation of UI Element manager.

#include "LaurelEyeEngine/UI/UIElementManager.h"
#include "LaurelEyeEngine/events/EventManager.h"

namespace LaurelEye {
    UIElementManager::~UIElementManager() {
        removeAllUIElements();
        clearInputMaps();
    }

    void UIElementManager::update(float deltaTime) {
        if ( !currentComponent || !inputManager || elements.empty() ) {
            return;
        }

        for ( const auto& it : elements ) {
            auto [mouseX, mouseY] = mouseToUICoordinate();
            if ( it.second->MouseInRange(mouseX, mouseY) && it.second->isActive() ) {
                setCurrentUIComponent(it.second);
                if ( inputManager->isMouseButtonPressed(mouseActivateButton) && currentComponent->CanBeActivated() ) {
                    dispatchActivateEvent();
                }
            }
        }

        hoverDelay += deltaTime;
        if ( hoverDelay >= hoverCooldown ) {
            if ( inputManager->isKeyHeld(leftKey) || inputManager->getGamepadAxis(GamepadAxes::gamepadLStickX) < -0.2 ) {
                toLeft();
                hoverDelay = 0.0f;
            }
            else if ( inputManager->isKeyHeld(rightKey) || inputManager->getGamepadAxis(GamepadAxes::gamepadLStickX) > 0.2 ) {
                toRight();
                hoverDelay = 0.0f;
            }
            else if ( inputManager->isKeyHeld(upKey) || inputManager->getGamepadAxis(GamepadAxes::gamepadLStickY) > 0.2 ) {
                toUp();
                hoverDelay = 0.0f;
            }
            else if ( inputManager->isKeyHeld(downKey) || inputManager->getGamepadAxis(GamepadAxes::gamepadLStickY) < -0.2 ) {
                toDown();
                hoverDelay = 0.0f;
            }

            // Handle input for current focused element
            if ( currentComponent && (inputManager->isKeyHeld(activateKey) || inputManager->isButtonHeld(activateButton)) ) {
                dispatchActivateEvent();
                hoverDelay = 0.0f;
            }
        }
    }

    void UIElementManager::registerUIElement(Graphics::UIComponent* elementToRegister) {
        assert(elementToRegister && "Could not register UIComponent to UIElementManager");
        elements[elementToRegister->GetUIName()] = elementToRegister;
        if ( currentComponent == nullptr ) {
            setCurrentUIComponent(elementToRegister);
        }
        std::cout << "Registered UI Element: " << elementToRegister->GetUIName() << std::endl;
    }

    void UIElementManager::deregisterUIElement(Graphics::UIComponent* elementToDeregister) {
        assert(elementToDeregister && "Could not deregister UIComponent from UIElementManager");
        elements.erase(elementToDeregister->GetUIName());
        if ( currentComponent == elementToDeregister ) {
            if ( elements.empty() ) {
                currentComponent = nullptr;
            }
            else {
                currentComponent = elements.begin()->second;
            }
        }
    }

    void UIElementManager::removeAllUIElements() {
        elements.clear();
        currentComponent = nullptr;
    }

    void UIElementManager::clearInputMaps() {
        while ( !inputMaps.empty() ) {
            inputMaps.pop();
        }
        for ( const auto& [key, val] : inputMapStorage ) {
            delete val;
        }
        inputMapStorage.clear();
    }

    void UIElementManager::pushInputMap(const std::string& inputMapName) {
        auto it = inputMapStorage.find(inputMapName);
        if ( inputMaps.empty() ) {
            if ( it != inputMapStorage.end() ) {
                inputMaps.push(it->second);
                std::unordered_map<std::string, SingleUIMapping*> map = inputMaps.top()->getInputMap();
                for ( auto& [key, val] : map ) {
                    if ( elements[key] != nullptr ) {
                        elements[key]->setIsActive(true);
                    }
                }
            }
            else {
                std::cerr << "UIElementManager: InputMap " << inputMapName << " not found!" << std::endl;
            }
            return;
        }
        if ( it != inputMapStorage.end() ) {
            std::unordered_map<std::string, SingleUIMapping*> map = inputMaps.top()->getInputMap();
            for ( auto& [key, val] : map ) {
                if ( elements[key] != nullptr ) {
                    elements[key]->setIsActive(false);
                }
            }

            inputMaps.push(it->second);
            map = inputMaps.top()->getInputMap();
            for ( auto& [key, val] : map ) {
                if ( elements[key] != nullptr ) {
                    elements[key]->setIsActive(true);
                }
            }
        }
        else {
            std::cerr << "UIElementManager: InputMap " << inputMapName << " not found!" << std::endl;
        }
    }

    void UIElementManager::popInputMap() {
        if ( inputMaps.size() > 1 ) {
            std::unordered_map<std::string, SingleUIMapping*> map = inputMaps.top()->getInputMap();
            for ( auto& [key, val] : map ) {
                elements[key]->setIsActive(false);
            }
            inputMaps.pop();

            if ( !inputMaps.empty() ) {
                currentComponent = elements[inputMaps.top()->getCurrent()];
                map = inputMaps.top()->getInputMap();
                for ( auto& [key, val] : map ) {
                    elements[key]->setIsActive(true);
                }
            }
            else {
                std::cerr << "UIElementManager: No InputMap after pop!" << std::endl;
            }
        }
        else {
            std::cerr << "UIElementManager: No InputMap to pop!" << std::endl;
        }
    }

    void UIElementManager::registerInputMap(UILayout* inputMap) {
        inputMapStorage[inputMap->getName()] = inputMap;
        // the first input map is the base map
        if ( inputMaps.empty() ) {
            pushInputMap(inputMap->getName());
        }
    }

    void UIElementManager::deregisterInputMap(UILayout* inputMap) {
        auto it = inputMapStorage.find(inputMap->getName());
        if ( it != inputMapStorage.end() ) {
            inputMapStorage.erase(it);
        }
    }

    std::pair<float, float> UIElementManager::mouseToUICoordinate() {
        if ( !inputManager || !window ) {
            return {0.0f, 0.0f};
        }
        float x = inputManager->getMousePosition().first;
        float y = inputManager->getMousePosition().second;
        x = 2.0f * (x / window->getWidth()) - 1.0f;
        y = 1.0f - 2.0f * (y / window->getHeight());
        return {x, y};
    }

    std::pair<float, float> UIElementManager::uiToMouseCoordinate() {
        if ( !inputManager || !window ) {
            return {0.0f, 0.0f};
        }
        float x = inputManager->getMousePosition().first;
        float y = inputManager->getMousePosition().second;
        x = (x + 1.0f) * 0.5f * window->getWidth();
        y = (1.0f - y) * 0.5f * window->getHeight();
        return {x, y};
    }

    void UIElementManager::toLeft() {
        if ( inputMaps.empty() ) {
            return;
        }
        if ( elements.find(inputMaps.top()->getLeft()) == elements.end() ) {
            std::cerr << "UIElementManager: No UI Element found for Left navigation!" << std::endl;
            return;
        }
        auto it = elements.find(inputMaps.top()->getLeft());
        if ( it == elements.end() ) {
            return;
        }
        else {
            setCurrentUIComponent(it->second);
        }
    }

    void UIElementManager::toRight() {
        if ( inputMaps.empty() ) {
            return;
        }
        if ( elements.find(inputMaps.top()->getRight()) == elements.end() ) {
            std::cerr << "UIElementManager: No UI Element found for Right navigation!" << std::endl;
            return;
        }
        auto it = elements.find(inputMaps.top()->getRight());
        if ( it == elements.end() ) {
            return;
        }
        else {
            setCurrentUIComponent(it->second);
        }
    }

    void UIElementManager::toUp() {
        if ( inputMaps.empty() ) {
            return;
        }
        if ( elements.find(inputMaps.top()->getUp()) == elements.end() ) {
            std::cerr << "UIElementManager: No UI Element found for Up navigation!" << std::endl;
            return;
        }
        auto it = elements.find(inputMaps.top()->getUp());
        if ( it == elements.end() ) {
            return;
        }
        else {
            setCurrentUIComponent(it->second);
        }
    }

    void UIElementManager::toDown() {
        if ( inputMaps.empty() ) {
            return;
        }
        if ( elements.find(inputMaps.top()->getDown()) == elements.end() ) {
            std::cerr << "UIElementManager: No UI Element found for Down navigation!" << std::endl;
            return;
        }
        auto it = elements.find(inputMaps.top()->getDown());
        if ( it == elements.end() ) {
            return;
        }
        else {
            setCurrentUIComponent(it->second);
        }
    }

    void UIElementManager::dispatchActivateEvent() {
        if ( !context ) {
            // There is no Context and Therefore cannot generate Events
            return;
        }
        auto* eventManager = context->getService<EventManager>();
        if ( !eventManager )
            throw std::runtime_error("No Event Manager");
        ButtonEvent evt(currentComponent->GetUIName());
        eventManager->broadcastEvent<ButtonEvent>(evt);
    }

    void UIElementManager::setCurrentUIComponent(Graphics::UIComponent* component) {
        if ( currentComponent ) {
            currentComponent->SetIsFocused(false);
        }
        currentComponent = component;
        if ( currentComponent ) {
            currentComponent->SetIsFocused(true);
            if (inputMaps.empty()) {
                return;
            }
            else {
                inputMaps.top()->setCurrent(currentComponent->GetUIName());
            }
        }
    }

    void UIElementManager::setInputAndWindow(LaurelEye::InputManager* inputMgr, LaurelEye::IWindow* _window) {
        inputManager = inputMgr;
        window = _window;
        std::cout << "UIElementManager InputManager and Window set" << std::endl;
    }

    void UIElementManager::registerListener() {
        auto* eventManager = context->getService<EventManager>();
        listener = eventManager->addListener<ButtonEvent>([&](const ButtonEvent& event) {
            std::cout << "[Button] Event Received: Button " << event.GetData() << " pressed." << std::endl;
        });
    }

    void UIElementManager::deregisterListener() {
        auto* eventManager = context->getService<EventManager>();
        assert(eventManager && "PhysicsSystem: No EventManager in EngineContext");
        eventManager->removeListener<ButtonEvent>(listener);
    }

} // namespace LaurelEye
