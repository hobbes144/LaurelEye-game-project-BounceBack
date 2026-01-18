/// @file   Spline.cpp
/// @author Martin Chow (chowmartin387@gmail.com)
/// @par    **DigiPen Email**
///     martin.chow@digipen.edu
/// @par    **Course**
///     GAM541
/// @date   01-14-2026
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#include "LaurelEyeEngine/math/Spline.h"

namespace LaurelEye {
    Vector2 Spline::evaluateArcLength(float u) {
        arcLengthTable.buildIfNeeded(*this);
        return arcLengthTable.evaluateArcLength(*this, u);
    }
    float Spline::getTotalLength() {
        arcLengthTable.buildIfNeeded(*this);
        return arcLengthTable.getTotalLength();
    }
    const std::vector<Vector2>& Spline::getControlPoints() {
        return controlPoints;
    }

    void Spline::setControlPoints(const std::vector<Vector2>& pts) {
        controlPoints = pts;
        update();
    }

    void Spline::clearControlPoints() {
        controlPoints.clear();
        arcLengthTable.clearArcLengthTable();
        update();
    }
} // namespace LaurelEye
