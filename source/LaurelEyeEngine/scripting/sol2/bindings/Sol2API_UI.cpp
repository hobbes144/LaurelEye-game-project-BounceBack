#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_UI.h"
#include "LaurelEyeEngine/events/EventManager.h"
#include "LaurelEyeEngine/graphics/graphics_components/UIButtonComponent.h"
#include "LaurelEyeEngine/UI/UIElementManager.h"

namespace LaurelEye::Scripting {
    void Sol2API_UI::setup(sol::state& lua, EngineContext* ctx) {
        auto* eventManager = ctx->getService<EventManager>();
        if ( !eventManager )
            throw std::runtime_error("Sol2API_UI: EventManager not available!");

        // -------------------------------------------------------
        // Expose UIButtonComponent to Lua
        // -------------------------------------------------------
        lua.new_usertype<LaurelEye::Graphics::UIButtonComponent>(
            "UIButtonComponent",
            sol::no_constructor,
            "getName", &LaurelEye::Graphics::UIComponent::GetUIName,
            "setFocused", &LaurelEye::Graphics::UIComponent::SetIsFocused,
            "isActive", &LaurelEye::Graphics::UIComponent::isActive,
            "setActive", &LaurelEye::Graphics::UIComponent::setIsActive);

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

    void Sol2API_UI::shutdown(EngineContext* ctx) {
        auto* eventManager = ctx->getService<EventManager>();
        if ( eventManager ) {
            eventManager->removeListener<ButtonEvent>(listenerID);
        }

        buttonCallbacks.clear();
    }
} // namespace LaurelEye::Scripting
