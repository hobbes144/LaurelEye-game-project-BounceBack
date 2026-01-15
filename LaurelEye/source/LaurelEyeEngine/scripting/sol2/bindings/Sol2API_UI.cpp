#include "LaurelEyeEngine/graphics/graphics_components/UIButtonComponent.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_UI.h"


namespace LaurelEye::Scripting {
    void Sol2API_UI::setup(sol::state& lua, EngineContext* ctx) {
        auto* eventManager = ctx->getService<EventManager>();
        if ( !eventManager )
            throw std::runtime_error("Sol2API_UI: EventManager not available!");

        auto* uiElementManager = ctx->getService<UIElementManager>();
        if ( !uiElementManager ) {
            throw std::runtime_error("Sol2API_UI: UIManager not available!");
        }

        setupUILayout(lua);
        setupUIComponent(lua);
        setupUIButtons(lua);
        setupButtonCallbacks(lua);
        setupEventListener(lua, eventManager);
        setupUIManager(lua, uiElementManager);
    }

    void Sol2API_UI::setupUIComponent(sol::state& lua) {
        // -------------------------------------------------------
        // Expose UIComponent to Lua
        // -------------------------------------------------------
        lua.new_usertype<LaurelEye::Graphics::UIComponent>(
            "UIComponent",
            sol::no_constructor,
            "getName", &LaurelEye::Graphics::UIComponent::GetUIName,
            "setFocused", &LaurelEye::Graphics::UIComponent::SetIsFocused,
            "setCanBeFocused", &LaurelEye::Graphics::UIComponent::SetCanBeFocused,
            "isActive", &LaurelEye::Graphics::UIComponent::isActive,
            "setActive", &LaurelEye::Graphics::UIComponent::setIsActive
        );
    }

    void Sol2API_UI::setupUIButtons(sol::state& lua) {
        // -------------------------------------------------------
        // Expose UIButtonComponent to Lua
        // -------------------------------------------------------
        lua.new_usertype<LaurelEye::Graphics::UIButtonComponent>(
            "UIButtonComponent",
            sol::no_constructor,
            "getName", &LaurelEye::Graphics::UIComponent::GetUIName,
            "setFocused", &LaurelEye::Graphics::UIComponent::SetIsFocused,
            "setCanBeFocused", &LaurelEye::Graphics::UIComponent::SetCanBeFocused,
            "isActive", &LaurelEye::Graphics::UIComponent::isActive,
            "setActive", &LaurelEye::Graphics::UIComponent::setIsActive
        );
    }

    void Sol2API_UI::setupUILayout(sol::state& lua) {
        lua.new_usertype<LaurelEye::SingleUIMapping>(
            "SingleUIMapping",
            sol::default_constructor,
            "setCurrent", &LaurelEye::SingleUIMapping::setCurrent,
            "getCurrent", &LaurelEye::SingleUIMapping::getCurrent,
            "setLeft", &LaurelEye::SingleUIMapping::setLeft,
            "getLeft", &LaurelEye::SingleUIMapping::getLeft,
            "setRight", &LaurelEye::SingleUIMapping::setRight,
            "getRight", &LaurelEye::SingleUIMapping::getRight,
            "setUp", &LaurelEye::SingleUIMapping::setUp,
            "getUp", &LaurelEye::SingleUIMapping::getUp,
            "setDown", &LaurelEye::SingleUIMapping::setDown,
            "getDown", &LaurelEye::SingleUIMapping::getDown
        );

        lua.new_usertype<LaurelEye::UILayout>(
            "UILayout",
            sol::default_constructor,
            "addMappingUnit", &LaurelEye::UILayout::addMappingUnit,
            "deleteMappingUnit", sol::overload(
                static_cast<void (LaurelEye::UILayout::*)(const std::string&)>(&LaurelEye::UILayout::deleteMappingUnit),
                static_cast<void (LaurelEye::UILayout::*)(LaurelEye::SingleUIMapping*)>(&LaurelEye::UILayout::deleteMappingUnit)),
            "setName", &LaurelEye::UILayout::setName,
            "getName", &LaurelEye::UILayout::getName
        );
    }

    void Sol2API_UI::setupButtonCallbacks(sol::state& lua) {
        // -------------------------------------------------------
        // Create global UI table in Lua
        // -------------------------------------------------------
        sol::table uiTable = lua.create_named_table("UI");

        // Store callbacks
        uiTable.set_function("onButtonPressed",
                             [&](const std::string& name, sol::function func) {
                                 buttonCallbacks[name].push_back(func);
                             });

        uiTable.set_function("clearButtonCallbacks",
                             [&](const std::string& name) {
                                 buttonCallbacks[name].clear();
                             });
    }

    void Sol2API_UI::setupEventListener(sol::state& lua, EventManager* eventManager) {
        // -------------------------------------------------------
        // Register event listener to forward events to Lua
        // -------------------------------------------------------
        listenerID = eventManager->addListener<ButtonEvent>(
            [&](const ButtonEvent& evt) {
                const std::string& btn = evt.GetData();

                auto it = buttonCallbacks.find(btn);
                if ( it == buttonCallbacks.end() )
                    return;

                for ( auto& callback : it->second ) {
                    if ( callback.valid() )
                        callback(evt.GetData());
                }
            });
    }

    void Sol2API_UI::setupUIManager(sol::state& lua, UIElementManager* uiManager) {
        // Expose UIElementManager class
        lua.new_usertype<UIElementManager>(
            "UIElementManager",
            sol::no_constructor,
            "getCurrentUIComponent", &UIElementManager::getCurrentUIComponent,
            "findUIComponent", &UIElementManager::findUIComponent,
            "registerUIElement", &UIElementManager::registerUIElement,
            "deregisterUIElement", &UIElementManager::deregisterUIElement,
            "registerInputMap", &UIElementManager::registerInputMap,
            "deregisterInputMap", &UIElementManager::deregisterInputMap,
            "pushInputMap", &UIElementManager::pushInputMap,
            "popInputMap", &UIElementManager::popInputMap,
            "setUIWindow", &UIElementManager::setUIWindow,
            "setInputManager", &UIElementManager::setInputManager,
            "enableCurrentUI", &UIElementManager::enableCurrentUI,
            "disableCurrentUI", &UIElementManager::disableCurrentUI,
            "disableCurrentFocusableUI", &UIElementManager::disableCurrentFocusableUI,
            "disableAllUI", &UIElementManager::disableAllUI,
            "setActivateButton", &UIElementManager::setActivateButton,
            "setEscapeButton", &UIElementManager::setEscapeButton,
            "setMouseActivateButton", &UIElementManager::setMouseActivateButton,
            "setActivateKey", &UIElementManager::setActivateKey,
            "setUpKey", &UIElementManager::setUpKey,
            "setDownKey", &UIElementManager::setDownKey,
            "setLeftKey", &UIElementManager::setLeftKey,
            "setRightKey", &UIElementManager::setRightKey,
            "setEscapeKey", &UIElementManager::setEscapeKey,
            "setIsUIActive", &UIElementManager::setIsUIActive,
            "getIsUIActive", &UIElementManager::getIsUIActive,
            "setDisableAllPrevious", &UIElementManager::setDisableAllPrevious,
            "getDisableAllPrevious", &UIElementManager::getDisableAllPrevious
        );

        // Expose the instance globally
        lua["UIElementManager"] = uiManager;
    }

    void Sol2API_UI::shutdown(EngineContext* ctx) {
        auto* eventManager = ctx->getService<EventManager>();
        if ( eventManager ) {
            eventManager->removeListener<ButtonEvent>(listenerID);
        }

        buttonCallbacks.clear();
    }
} // namespace LaurelEye::Scripting
