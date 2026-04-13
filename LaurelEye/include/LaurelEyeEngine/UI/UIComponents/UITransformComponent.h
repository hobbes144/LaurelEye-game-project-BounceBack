/// @file   UIButtonComponent.h
/// @author Daoming Wang
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   10-30-2025
/// @brief  UI button component for the LaurelEye engine.

#pragma once
#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/math/Rect.h"

namespace LaurelEye::UI {

    struct UITransformData {
        Vector2 anchorMin = {0.5f, 0.5f};
        Vector2 anchorMax = {0.5f, 0.5f};

        Vector2 pivot = {0.5f, 0.5f};

        Vector2 localOffset = {0.0f, 0.0f};

        Vector2 size = {100.0f, 100.0f};
    };

    class UITransformComponent : public IComponent {
    public:
        UITransformComponent() = default;
        UITransformComponent(UITransformData data)
            : uiTransData(data), dirty(true) {}

        // Dirty
        void MarkDirty();
        bool IsDirty() const;

        // Resolved
        const Rect& GetResolvedRect() const;
        void SetResolvedRect(const Rect& r);

        // Core Layout
        const Vector2& GetSize() const;
        void SetSize(const Vector2& s);

        const Vector2& GetLocalOffset() const;
        void SetLocalOffset(const Vector2& o);

        const Vector2& GetAnchorMin() const;
        void SetAnchorMin(const Vector2& a);

        const Vector2& GetAnchorMax() const;
        void SetAnchorMax(const Vector2& a);

        const Vector2& GetPivot() const;
        void SetPivot(const Vector2& p);

        const UITransformData& GetUITransformData() const;

        // Helpers
        void SetAnchors(const Vector2& a);
        void Center();
        void Stretch();

    private:
        UITransformData uiTransData;
        Rect resolvedRect;
        bool dirty = true;

    };
} // namespace LaurelEye::Graphics
