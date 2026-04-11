#pragma once

#include "LaurelEyeEngine/math/VectorTemplated.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"
#include "LaurelEyeEngine/graphics/resources/Font.h"
#include <memory>
#include <string>

namespace LaurelEye::UI {

    enum class TextAlignment {
        Left,
        Center,
        Right
    };

    class UITextComponent : public Graphics::IRenderableComponent {

    public:
        UITextComponent() : Graphics::IRenderableComponent(Graphics::RenderComponentType::UITextRenderable, {}) {}

        // --- Text Content ---
        void SetText(const std::string& t);
        const std::string& GetText() const;

        // --- Font ---
        void SetFontName(std::string fontName);
        std::string GetFontName() const;

        // --- Font Size ---
        void SetFontSize(float size);
        float GetFontSize() const;

        // --- Color ---
        void SetColor(const Vector4& c);
        const Vector4& GetColor() const;

        // --- Transparency ---
        void SetTransparency(float t);
        float GetTransparency() const;

        // --- Alignment ---
        void SetAlignment(TextAlignment a);
        TextAlignment GetAlignment() const;

        // --- Wrapping ---
        void SetWrap(bool w);
        bool GetWrap() const;

        void SetMaxWidth(float w);
        float GetMaxWidth() const;

        // --- Transform Binding ---
        void BindTransform() override;
        const UITransformComponent* GetBoundTransform() const;

    private:
        std::string text;
        std::string fontName;
        float fontSize = 16.0f;

        Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
        float transparency = 1.0f;

        TextAlignment alignment = TextAlignment::Left;
        bool wrap = false;
        float maxWidth = 0.0f;

        UITransformComponent* uiTransform = nullptr;
    };

} // namespace LaurelEye::UI
