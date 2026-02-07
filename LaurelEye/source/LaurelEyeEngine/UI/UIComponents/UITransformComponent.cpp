#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"

namespace LaurelEye::UI {

    bool UITransformComponent::IsDirty() const {
        return dirty;
    }

    void UITransformComponent::MarkDirty() {
        dirty = true;
    }

    const Rect& UITransformComponent::GetResolvedRect() const {
        return resolvedRect;
    }

    void UITransformComponent::SetResolvedRect(const Rect& r) {
        resolvedRect = r;
        MarkDirty();
    }

    const Vector2& UITransformComponent::GetSize() const {
        return uiTransData.size;
    }
    void UITransformComponent::SetSize(const Vector2& s) {
        uiTransData.size = s;
        MarkDirty();
    }

    const Vector2& UITransformComponent::GetLocalOffset() const {
        return uiTransData.localOffset;
    }
    void UITransformComponent::SetLocalOffset(const Vector2& o) {
        uiTransData.localOffset = o;
        MarkDirty();
    }

    const Vector2& UITransformComponent::GetAnchorMin() const {
        return uiTransData.anchorMin;
    }
    void UITransformComponent::SetAnchorMin(const Vector2& a) {
        uiTransData.anchorMin = a;
        MarkDirty();
    }

    const Vector2& UITransformComponent::GetAnchorMax() const {
        return uiTransData.anchorMax;
    }
    void UITransformComponent::SetAnchorMax(const Vector2& a) {
        uiTransData.anchorMax = a;
        MarkDirty();
    }

    const Vector2& UITransformComponent::GetPivot() const {
        return uiTransData.pivot;
    }
    void UITransformComponent::SetPivot(const Vector2& p) {
        uiTransData.pivot = p;
        MarkDirty();
    }

    const UITransformData& UITransformComponent::GetUITransformData() const {
        return uiTransData;
    }

    void UITransformComponent::SetAnchors(const Vector2& a) {
        uiTransData.anchorMin = a;
        uiTransData.anchorMax = a;
        MarkDirty();
    }

    void UITransformComponent::Center() {
        uiTransData.anchorMin = {0.5f, 0.5f};
        uiTransData.anchorMax = {0.5f, 0.5f};
        uiTransData.pivot = {0.5f, 0.5f};
        MarkDirty();
    }

    void UITransformComponent::Stretch() {
        uiTransData.anchorMin = {0.0f, 0.0f};
        uiTransData.anchorMax = {1.0f, 1.0f};
        uiTransData.pivot = {0.5f, 0.5f};
        MarkDirty();
    }

}
