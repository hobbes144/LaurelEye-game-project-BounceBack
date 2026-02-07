#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_UI.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"


namespace LaurelEye::Scripting {
    void Sol2API_UI::setup(sol::state& lua, EngineContext* ctx) {
        setupUITransformComponent(lua);
        setupUIRenderComponent(lua);
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
            "isVisible", &LaurelEye::UI::UIRenderComponent::IsVisible
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
