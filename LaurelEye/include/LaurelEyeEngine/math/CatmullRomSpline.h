#pragma once

#pragma once
#include "Spline.h"
#include <vector>

namespace LaurelEye {
    // Catmull–Rom spline implementation.
    // Produces a smooth interpolating curve through all control points.
    class CatmullRomSpline : public Spline {

    public:
        CatmullRomSpline() = default;
        ~CatmullRomSpline() override = default;

        // Evaluate position on the spline at parameter t ∈ [0, 1].
        Vector2 evaluate(float t) const override;

        // Evaluate derivative (tangent) at parameter t.
        Vector2 derivative(float t) const override;

        // Optional: tension parameter (0.5 = standard Catmull–Rom)
        void setTension(float t) {
            tension = t;
        }
        float getTension() const { return tension; }

        // Access control points directly
        Vector2& operator[](int index) {
            return controlPoints[index];
        }

        const Vector2& operator[](int index) const {
            return controlPoints[index];
        }

        // Add/remove control points
        void addPoint(const Vector2& p) {
            controlPoints.push_back(p);
        }

        // Serialization
        void toJSON(const std::string& outStr) const override;
        void fromJSON(const std::string& inStr) override;

    private:
        // Catmull–Rom tension (0.5 = centripetal, 0 = uniform)
        float tension = 0.5f;

        // Internal helper: evaluate a segment between p1 and p2
        Vector2 evalSegment(int i, float t) const;
        Vector2 derivSegment(int i, float t) const;

        // Convert global t ∈ [0,1] to segment index + local t
        void mapTtoSegment(float t, int& segIndex, float& localT) const;

        void update() override;
    };

    static_assert(!std::is_abstract_v<CatmullRomSpline>, "CatmullRomSpline is STILL abstract — missing derivative() definition!");
} // namespace LaurelEye
