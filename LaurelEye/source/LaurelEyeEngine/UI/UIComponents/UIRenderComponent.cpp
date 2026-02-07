#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"

namespace LaurelEye::UI {

    //Binding Transform
    void UIRenderComponent::BindTransform() {
        // Intentionally empty — UI uses UITransformComponent
        uiTransComp = owner->findComponent<UITransformComponent>();
    }
    const UITransformComponent* UIRenderComponent::GetBoundTransform() const {
        return uiTransComp;
    }

    // Color
    void UIRenderComponent::SetColor(Vector4 c) {
        color = c;
    }
    const Vector4& UIRenderComponent::GetColor() const {
        return color;
    }

    // Transparency
    void UIRenderComponent::SetTransparency(float t) {
        transparency = t;
    }
    float UIRenderComponent::GetTransparency() const {
        return transparency;
    }

    // Layer
    int UIRenderComponent::GetLayer() const {
        return layer;
    }
    void UIRenderComponent::SetLayer(int l) {
        layer = l;
    }

    // Visibity
    void UIRenderComponent::SetVisible() {
        visible = true;
    }
    void UIRenderComponent::SetInvisible() {
        visible = false;
    }
    void UIRenderComponent::ToggleVisibility() {
        visible = !visible;
    }
    bool UIRenderComponent::IsVisible() const {
        return visible;
    }

    /*
    // Texture
    void UIRenderComponent::SetUseTexture(bool ut) {
        usingTexture = ut;
    }
    void UIRenderComponent::EnableTexture() {
        usingTexture = true;
    }
    void UIRenderComponent::DisableTexture() {
        usingTexture = false;
    }
    void UIRenderComponent::ToggleUseTexture() {
        usingTexture = !usingTexture;
    }
    bool UIRenderComponent::IsTextureEnabled() const {
        return usingTexture;
    }
    */

}
