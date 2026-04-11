#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_UI.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"

namespace LaurelEye::Scripting {
    void Sol2API_UI::setup(sol::state& lua, EngineContext* ctx) {
        setupUITransformComponent(lua);
        setupUIRenderComponent(lua);
        setupUIInteractionComponent(lua);
    }

    void Sol2API_UI::setupUITransformComponent(sol::state& lua) {

        lua.new_usertype<UI::UITransformComponent>(
            "UITransformComponent",

            // Constructors
            sol::constructors<
                LaurelEye::UI::UITransformComponent(),
                LaurelEye::UI::UITransformComponent(LaurelEye::UI::UITransformData)>(),

            // Dirty
            "markDirty", &LaurelEye::UI::UITransformComponent::MarkDirty,
            "isDirty", &LaurelEye::UI::UITransformComponent::IsDirty,

            // Resolved
            "getResolvedRect", &LaurelEye::UI::UITransformComponent::GetResolvedRect,

            // Core Layout
            "getSize", &LaurelEye::UI::UITransformComponent::GetSize,
            "setSize", &LaurelEye::UI::UITransformComponent::SetSize,

            "getLocalOffset", &LaurelEye::UI::UITransformComponent::GetLocalOffset,
            "setLocalOffset", &LaurelEye::UI::UITransformComponent::SetLocalOffset,

            "getAnchorMin", &LaurelEye::UI::UITransformComponent::GetAnchorMin,
            "setAnchorMin", &LaurelEye::UI::UITransformComponent::SetAnchorMin,

            "getAnchorMax", &LaurelEye::UI::UITransformComponent::GetAnchorMax,
            "setAnchorMax", &LaurelEye::UI::UITransformComponent::SetAnchorMax,

            "getPivot", &LaurelEye::UI::UITransformComponent::GetPivot,
            "setPivot", &LaurelEye::UI::UITransformComponent::SetPivot,

            // Helpers
            "setAnchors", &LaurelEye::UI::UITransformComponent::SetAnchors,
            "center", &LaurelEye::UI::UITransformComponent::Center,
            "stretch", &LaurelEye::UI::UITransformComponent::Stretch
        );

    }

    void Sol2API_UI::setupUIInteractionComponent(sol::state& lua) {

        // ============================================================
        // UIInteractionComponent
        // ============================================================

        lua.new_usertype<LaurelEye::UI::UIInteractionComponent>(
            "UIInteractionComponent",

            sol::constructors<
                LaurelEye::UI::UIInteractionComponent()>(),

            // --- Component Settings ---
            "getIsInteractable", &LaurelEye::UI::UIInteractionComponent::getIsInteractable,
            "setInteractable", &LaurelEye::UI::UIInteractionComponent::setInteractable,

            "getIsBlockingRaycasts", &LaurelEye::UI::UIInteractionComponent::getIsBlockingRaycasts,
            "setBlockingRaycasts", &LaurelEye::UI::UIInteractionComponent::setBlockingRaycasts,

            "getRecievingHover", &LaurelEye::UI::UIInteractionComponent::getRecievingHover,
            "setReceiveHover", &LaurelEye::UI::UIInteractionComponent::setReceiveHover,

            "getRecievingClick", &LaurelEye::UI::UIInteractionComponent::getRecievingClick,
            "setReceiveClick", &LaurelEye::UI::UIInteractionComponent::setReceiveClick,

            "getRecievingDrag", &LaurelEye::UI::UIInteractionComponent::getRecievingDrag,
            "setReceiveDrag", &LaurelEye::UI::UIInteractionComponent::setReceiveDrag,

            "getDragThreshold", &LaurelEye::UI::UIInteractionComponent::getDragThreshold,
            "setDragThreshold", &LaurelEye::UI::UIInteractionComponent::setDragThreshold,

            "getFocusable", &LaurelEye::UI::UIInteractionComponent::getFocusable,
            "setFocusable", &LaurelEye::UI::UIInteractionComponent::setFocusable,

            "getInteractionPriority", &LaurelEye::UI::UIInteractionComponent::getInteractionPriority,
            "setInteractionPriority", &LaurelEye::UI::UIInteractionComponent::setInteractionPriority,

            // --- Runtime State ---
            "getIsHovered", &LaurelEye::UI::UIInteractionComponent::getIsHovered,
            "setIsHovered", &LaurelEye::UI::UIInteractionComponent::setIsHovered,

            "getWasHovered", &LaurelEye::UI::UIInteractionComponent::getWasHovered,
            "setWasHovered", &LaurelEye::UI::UIInteractionComponent::setWasHovered,

            "getIsPressed", &LaurelEye::UI::UIInteractionComponent::getIsPressed,
            "setIsPressed", &LaurelEye::UI::UIInteractionComponent::setIsPressed,

            "getWasPressed", &LaurelEye::UI::UIInteractionComponent::getWasPressed,
            "setWasPressed", &LaurelEye::UI::UIInteractionComponent::setWasPressed,

            "getIsDragging", &LaurelEye::UI::UIInteractionComponent::getIsDragging,
            "setIsDragging", &LaurelEye::UI::UIInteractionComponent::setIsDragging,

            "getPressStartPos", &LaurelEye::UI::UIInteractionComponent::getPressStartPos,
            "setPressStartPos", &LaurelEye::UI::UIInteractionComponent::setPressStartPos,

            "getDragLastPos", &LaurelEye::UI::UIInteractionComponent::getDragLastPos,
            "setDragLastPos", &LaurelEye::UI::UIInteractionComponent::setDragLastPos,

            "getDragOffset", &LaurelEye::UI::UIInteractionComponent::getDragOffset,
            "setDragOffset", &LaurelEye::UI::UIInteractionComponent::setDragOffset);

        // ============================================================
        // UI Event Type Enum
        // ============================================================

        lua.new_enum<LaurelEye::UI::UIInteractionEventData::UIEventType>(
            "UIEventType",
            {
                { "HoverEnter",  LaurelEye::UI::UIInteractionEventData::UIEventType::HoverEnter},
                {  "HoverExit",   LaurelEye::UI::UIInteractionEventData::UIEventType::HoverExit},
                {    "Pressed",     LaurelEye::UI::UIInteractionEventData::UIEventType::Pressed},
                {   "Released",    LaurelEye::UI::UIInteractionEventData::UIEventType::Released},
                {    "Clicked",     LaurelEye::UI::UIInteractionEventData::UIEventType::Clicked},
                {       "Held",        LaurelEye::UI::UIInteractionEventData::UIEventType::Held},
                {  "DragStart",   LaurelEye::UI::UIInteractionEventData::UIEventType::DragStart},
                {   "Dragging",    LaurelEye::UI::UIInteractionEventData::UIEventType::Dragging},
                {    "DragEnd",     LaurelEye::UI::UIInteractionEventData::UIEventType::DragEnd},
                {"FocusGained", LaurelEye::UI::UIInteractionEventData::UIEventType::FocusGained},
                {  "FocusLost",   LaurelEye::UI::UIInteractionEventData::UIEventType::FocusLost}
        });

        // ============================================================
        // UIInteractionEventData
        // ============================================================

        lua.new_usertype<LaurelEye::UI::UIInteractionEventData>(
            "UIInteractionEventData",

            // --- Basic Info ---
            "type", &LaurelEye::UI::UIInteractionEventData::type,
            "entityID", &LaurelEye::UI::UIInteractionEventData::entityID,
            "entity", &LaurelEye::UI::UIInteractionEventData::entityRef,

            // --- Positions ---
            "screenPosition", &LaurelEye::UI::UIInteractionEventData::screenPosition,
            "localPosition", &LaurelEye::UI::UIInteractionEventData::localPosition,

            // --- Input Info ---
            "button", &LaurelEye::UI::UIInteractionEventData::button,
            "shift", &LaurelEye::UI::UIInteractionEventData::shift,
            "ctrl", &LaurelEye::UI::UIInteractionEventData::ctrl,
            "alt", &LaurelEye::UI::UIInteractionEventData::alt,

            // --- Drag Info ---
            "dragDelta", &LaurelEye::UI::UIInteractionEventData::dragDelta,

            // --- Time ---
            "timeStamp", &LaurelEye::UI::UIInteractionEventData::timeStamp);

    }

    void Sol2API_UI::setupUIRenderComponent(sol::state& lua) {

        lua.new_usertype<UI::UIRenderComponent>(
            "UIRender",

            sol::constructors<LaurelEye::UI::UIRenderComponent()>(),

            // Transform binding
            "bindTransform", &LaurelEye::UI::UIRenderComponent::BindTransform,
            "getTransform", &LaurelEye::UI::UIRenderComponent::GetBoundTransform,

            // Color
            "setColor", &LaurelEye::UI::UIRenderComponent::SetColor,
            "getColor", &LaurelEye::UI::UIRenderComponent::GetColor,

            // Transparency
            "setTransparency", &LaurelEye::UI::UIRenderComponent::SetTransparency,
            "getTransparency", &LaurelEye::UI::UIRenderComponent::GetTransparency,

            // Layer
            "getLayer", &LaurelEye::UI::UIRenderComponent::GetLayer,
            "setLayer", &LaurelEye::UI::UIRenderComponent::SetLayer,

            // Visibility
            "setVisible", &LaurelEye::UI::UIRenderComponent::SetVisible,
            "setInvisible", &LaurelEye::UI::UIRenderComponent::SetInvisible,
            "toggleVisibility", &LaurelEye::UI::UIRenderComponent::ToggleVisibility,
            "isVisible", &LaurelEye::UI::UIRenderComponent::IsVisible,

            sol::base_classes, sol::bases<Graphics::IRenderableComponent>()
        );

    }

    /*
    void Sol2API_UI::setupButtonCallbacks(sol::state& lua) {

    }

    void Sol2API_UI::setupEventListener(sol::state& lua, EventManager* eventManager) {
        
    }
    */

    void Sol2API_UI::shutdown(EngineContext* ctx) {

    }
} // namespace LaurelEye::Scripting
