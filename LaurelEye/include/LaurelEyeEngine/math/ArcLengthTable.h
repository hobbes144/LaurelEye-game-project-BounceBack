#pragma once
#include <vector>
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye {

    class Spline;
    using Vector2 = VectorTemplated<float, 2>;

    class ArcLengthTable {

    public:
        ArcLengthTable() = default;

        // Build the table for a given spline
        void build(const Spline& spline, int resolution);

        // Convert distance → normalized t
        float distanceToT(float distance) const;

        // Evaluate the spline at a given arc-length distance
        Vector2 evaluateArcLength(const Spline& spline, float distance) const;

        float getTotalLength() const { return totalLength; }

        void buildIfNeeded(const Spline& spline, int resolution = 10000);

        void clearArcLengthTable();

    private:
        // store the t value, so that we can do interpolation to create smooth curve
        std::vector<float> parameterValues;
        std::vector<Vector2> sampledPoints;
        std::vector<float> cumulativeDistances;
        float totalLength;
    };

    // test default constructible for lua binding
    static_assert(std::is_default_constructible_v<ArcLengthTable>, "ArcLengthTable is NOT default constructible");
} // namespace LaurelEye
