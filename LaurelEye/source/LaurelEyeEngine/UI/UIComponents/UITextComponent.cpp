#include "LaurelEyeEngine/UI/UIComponents/UITextComponent.h"

namespace LaurelEye::UI {

    // --- Transform Binding ---
    void UITextComponent::BindTransform() {
        // Same pattern as UIRenderComponent
        uiTransform = owner->findComponent<UITransformComponent>();
    }

    const UITransformComponent* UITextComponent::GetBoundTransform() const {
        return uiTransform;
    }

    // --- Text Content ---
    void UITextComponent::SetText(const std::string& t) {
        text = t;
    }

    const std::string& UITextComponent::GetText() const {
        return text;
    }

    // --- Font ---
    void UITextComponent::SetFontName(std::string _fontName) {
        fontName = _fontName;
    }

    std::string UITextComponent::GetFontName() const {
        return fontName;
    }

    // --- Font Size ---
    void UITextComponent::SetFontSize(float size) {
        fontSize = size;
    }

    float UITextComponent::GetFontSize() const {
        return fontSize;
    }

    // --- Color ---
    void UITextComponent::SetColor(const Vector4& c) {
        color = c;
    }

    const Vector4& UITextComponent::GetColor() const {
        return color;
    }

    // --- Transparency ---
    void UITextComponent::SetTransparency(float t) {
        transparency = t;
    }

    float UITextComponent::GetTransparency() const {
        return transparency;
    }

    // --- Alignment ---
    void UITextComponent::SetAlignment(TextAlignment a) {
        alignment = a;
    }

    TextAlignment UITextComponent::GetAlignment() const {
        return alignment;
    }

    // --- Wrapping ---
    void UITextComponent::SetWrap(bool w) {
        wrap = w;
    }

    bool UITextComponent::GetWrap() const {
        return wrap;
    }

    void UITextComponent::SetMaxWidth(float w) {
        maxWidth = w;
    }

    float UITextComponent::GetMaxWidth() const {
        return maxWidth;
    }

} // namespace LaurelEye::UI
