#pragma once
#include "LaurelEyeEngine/math/VectorTemplated.h"
#include "LaurelEyeEngine/math/ArcLengthTable.h"
#include <iostream>


namespace LaurelEye {

    class Vector3;

    using Vector2 = VectorTemplated<float, 2>;

    class Spline {
        
    protected:
        std::vector<Vector2> controlPoints;
        ArcLengthTable arcLengthTable;

    private:
        
        // update the arc length table when control points change
        virtual void update() =0;

    public:
        Spline() = default;

        virtual ~Spline() = default;

        // Evaluate the spline at parameter t ∈ [0, 1].
        // This is the standard (non‑arc‑length‑corrected) evaluation.
        virtual Vector2 evaluate(float t) const = 0;

        // First derivative of the spline at parameter t.
        // Useful for tangent vectors, curvature, etc.
        virtual Vector2 derivative(float t) const = 0;

        // Evaluate the spline using arc‑length parameterization.
        // u ∈ [0, totalLength], giving constant‑speed traversal.
        Vector2 evaluateArcLength(float u);

        // Returns the total arc length of the spline.
        float getTotalLength();

        // Returns the current control points (likely should return const&).
        const std::vector<Vector2>& getControlPoints();

        // Control point related methods
        // Control points setter.
        void setControlPoints(const std::vector<Vector2>& pts);

        // Non-const access (allows modification)
        Vector2& operator[](int index) {
            return controlPoints[index];
        }

        // Const access (read-only)
        const Vector2& operator[](int index) const {
            return controlPoints[index];
        }

        Vector2& at(int index) {
            if ( index < 0 || index >= controlPoints.size() ) throw std::out_of_range("Spline control point index out of range");
            return controlPoints[index];
        }

        void clearControlPoints();

        // Serialization maybe not virtual later
        virtual void toJSON(const std::string& outStr) const =0;
        virtual void fromJSON(const std::string& outStr)=0;
    };

    #include <type_traits>

} // namespace LaurelEye
