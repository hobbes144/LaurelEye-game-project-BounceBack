///
/// @file   Rect.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   01-16-2026
/// @brief
///

#pragma once

#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye {

    struct Rect {
        Vector2 center = {0.0f, 0.0f}; // Top Left
        Vector2 size = {0.0f, 0.0f};     // Width Height

        Rect() = default;

        Rect(const Vector2& c, const Vector2& sz)
            : center(c), size(sz) {}

        // Edges
        float Left() const { return center[0] - size[0] * 0.5f; }
        float Right() const { return center[0] + size[0] * 0.5f; }
        float Bottom() const { return center[1] - size[1] * 0.5f; }
        float Top() const { return center[1] + size[1] * 0.5f; }

        // Corners
        Vector2 TopLeft() const { return {Left(), Top()}; }
        Vector2 TopRight() const { return {Right(), Top()}; }
        Vector2 BottomLeft() const { return {Left(), Bottom()}; }
        Vector2 BottomRight() const { return {Right(), Bottom()}; }

        // Mutators
        void SetCenter(const Vector2& c) { center = c; }

        void Translate(const Vector2& delta) {
            center = center + delta;
        }

        void Inflate(float amount) {
            size = size + Vector2(amount * 2, amount * 2);
        }

        //Queries
        bool Contains(const Vector2& point) const {
            return point[0] >= Left() &&
                   point[0] <= Right() &&
                   point[1] >= Bottom() &&
                   point[1] <= Top();
        }

        bool Intersects(const Rect& other) const {
            return !(other.Left() > Right() ||
                     other.Right() < Left() ||
                     other.Bottom() > Top() ||
                     other.Top() < Bottom());
        }

        bool IsValid() const {
            return size[0] >= 0.0f && size[1] >= 0.0f;
        }
    };

} // namespace LaurelEye
