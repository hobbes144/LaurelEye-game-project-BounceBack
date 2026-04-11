#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_UIText.h"

#include "LaurelEyeEngine/UI/UIComponents/UITextComponent.h"


namespace LaurelEye::Scripting {

    void LaurelEye::Scripting::Sol2API_UIText::setup(sol::state& lua, EngineContext* ctx) {
        setupUITextComponent(lua);
    }

    void LaurelEye::Scripting::Sol2API_UIText::setupUITextComponent(sol::state& lua) {

        // ============================================================
        // TextAlignment Enum
        // ============================================================
        lua.new_enum<LaurelEye::UI::TextAlignment>(
            "TextAlignment",
            {
                {  "Left",   LaurelEye::UI::TextAlignment::Left},
                {"Center", LaurelEye::UI::TextAlignment::Center},
                { "Right",  LaurelEye::UI::TextAlignment::Right}
        });

        // ============================================================
        // UITextComponent
        // ============================================================
        lua.new_usertype<LaurelEye::UI::UITextComponent>(
            "UITextComponent",

            sol::constructors<
                LaurelEye::UI::UITextComponent()>(),

            // --- Text Content ---
            "setText", &LaurelEye::UI::UITextComponent::SetText,
            "getText", &LaurelEye::UI::UITextComponent::GetText,

            // --- Font ---
            "setFontName", &LaurelEye::UI::UITextComponent::SetFontName,
            "getFontName", &LaurelEye::UI::UITextComponent::GetFontName,

            // --- Font Size ---
            "setFontSize", &LaurelEye::UI::UITextComponent::SetFontSize,
            "getFontSize", &LaurelEye::UI::UITextComponent::GetFontSize,

            // --- Color ---
            "setColor", &LaurelEye::UI::UITextComponent::SetColor,
            "getColor", &LaurelEye::UI::UITextComponent::GetColor,

            // --- Transparency ---
            "setTransparency", &LaurelEye::UI::UITextComponent::SetTransparency,
            "getTransparency", &LaurelEye::UI::UITextComponent::GetTransparency,

            // --- Alignment ---
            "setAlignment", &LaurelEye::UI::UITextComponent::SetAlignment,
            "getAlignment", &LaurelEye::UI::UITextComponent::GetAlignment,

            // --- Wrapping ---
            "setWrap", &LaurelEye::UI::UITextComponent::SetWrap,
            "getWrap", &LaurelEye::UI::UITextComponent::GetWrap,

            "setMaxWidth", &LaurelEye::UI::UITextComponent::SetMaxWidth,
            "getMaxWidth", &LaurelEye::UI::UITextComponent::GetMaxWidth,

            // --- Transform Binding ---
            "bindTransform", &LaurelEye::UI::UITextComponent::BindTransform,
            "getBoundTransform", &LaurelEye::UI::UITextComponent::GetBoundTransform);
    }

} // namespace LaurelEye::Scripting
