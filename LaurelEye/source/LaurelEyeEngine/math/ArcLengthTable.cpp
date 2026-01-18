/// @file   Spline.cpp
/// @author Martin Chow (chowmartin387@gmail.com)
/// @par    **DigiPen Email**
///     martin.chow@digipen.edu
/// @par    **Course**
///     GAM541
/// @date   01-14-2026
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#include "LaurelEyeEngine/math/ArcLengthTable.h"
#include "LaurelEyeEngine/math/Spline.h"
#include <stack>
#include <map>

float distance(const LaurelEye::Vector2& a, const LaurelEye::Vector2& b) {
    return (b - a).magnitude();
}

namespace LaurelEye {
    void ArcLengthTable::build(const Spline& spline, int resolution) {
        sampledPoints.clear();
        cumulativeDistances.clear();
        parameterValues.clear();
        totalLength = 0.0f;

        // Special case: straight line detection
        // in straight lines we need to do uniform sampling otherwise
        // adaptive subdivision will not sample evenly(aka just 3 point)
        auto p0 = spline.evaluate(0.0f);
        auto p1 = spline.evaluate(1.0f);
        bool isStraight = true;
        for ( int i = 1; i < 4; ++i ) {
            float t = i / 4.0f;
            Vector2 p = spline.evaluate(t);
            // Check collinearity by comparing distances
            float d = distance(p0, p1);
            float d2 = distance(p0, p) + distance(p, p1);
            if ( std::abs(d2 - d) > 0.0001f ) {
                isStraight = false;
                break;
            }
        }

        // ---------------------------------------------------------
        // Straight line: uniform sampling
        // ---------------------------------------------------------
        if ( isStraight ) {
            float sum = 0.0f;
            sampledPoints.reserve(resolution + 1);
            cumulativeDistances.reserve(resolution + 1);
            parameterValues.reserve(resolution + 1);

            Vector2 prev = spline.evaluate(0.0f);
            sampledPoints.push_back(prev);
            cumulativeDistances.push_back(0.0f);
            parameterValues.push_back(0.0f);

            for ( int i = 1; i <= resolution; ++i ) {
                float t = i / float(resolution);
                Vector2 p = spline.evaluate(t);
                sum += distance(prev, p);
                sampledPoints.push_back(p);
                cumulativeDistances.push_back(sum);
                parameterValues.push_back(t);
                prev = p;
            }
            totalLength = sum;
            return;
        }

        // ---------------------------------------------------------
        // Curved spline: adaptive subdivision
        // ---------------------------------------------------------
        struct Seg {
            float a, b;
        };
        std::stack<Seg> stack;
        stack.push({0.0f, 1.0f});
        std::map<float, float> arcMap;
        arcMap[0.0f] = 0.0f;
        const float threshold = 0.000001f;

        while ( !stack.empty() ) {
            auto [ua, ub] = stack.top();
            stack.pop();
            float um = 0.5f * (ua + ub);
            Vector2 pA = spline.evaluate(ua);
            Vector2 pM = spline.evaluate(um);
            Vector2 pB = spline.evaluate(ub);
            float A = distance(pA, pM);
            float B = distance(pM, pB);
            float C = distance(pA, pB);
            float deviation = std::abs((A + B) - C);

            if ( deviation > threshold ) {
                stack.push({um, ub});
                stack.push({ua, um});
            }
            else {
                float sA = arcMap[ua];
                float sM = sA + A;
                float sB = sM + B;
                arcMap[um] = sM;
                if ( !arcMap.count(ub) )
                    arcMap[ub] = sB;
            }
        }

        // Convert map to sorted table
        sampledPoints.reserve(arcMap.size());
        cumulativeDistances.reserve(arcMap.size());
        parameterValues.reserve(arcMap.size());

        for ( auto& [u, s] : arcMap ) {
            parameterValues.push_back(u);
            sampledPoints.push_back(spline.evaluate(u));
            cumulativeDistances.push_back(s);
        }
        totalLength = cumulativeDistances.back();
    }

    // Convert distance to normalized t
    float ArcLengthTable::distanceToT(float distance) const {
        if ( cumulativeDistances.empty() )
            return 0.0f;
        if ( distance <= 0.0f )
            return 0.0f;
        if ( distance >= totalLength )
            return 1.0f;

        // Binary search for the segment containing the distance
        int low = 0;
        int high = static_cast<int>(cumulativeDistances.size()) - 1;
        while ( low < high ) {
            int mid = (low + high) / 2;
            if ( cumulativeDistances[mid] < distance )
                low = mid + 1;
            else
                high = mid;
        }

        int i = std::max(1, low);
        float d0 = cumulativeDistances[i - 1];
        float d1 = cumulativeDistances[i];

        // Linear interpolation within the segment
        float segmentLength = d1 - d0;
        float alpha = (segmentLength > 0.0f)
                        ? (distance - d0) / segmentLength
                        : 0.0f;

        // USE THE ACTUAL STORED PARAMETER VALUES to create smooth curve
        float t0 = parameterValues[i - 1];
        float t1 = parameterValues[i];

        return t0 + alpha * (t1 - t0);
    }

    Vector2 ArcLengthTable::evaluateArcLength(const Spline& spline, float distance) const {
        float t = distanceToT(distance);
        return spline.evaluate(t);
    }

    void ArcLengthTable::buildIfNeeded(const Spline& spline, int resolution) {
        if ( sampledPoints.empty() ) {
            build(spline, resolution);
        }
    }
    void ArcLengthTable::clearArcLengthTable() {
        sampledPoints.clear();
        cumulativeDistances.clear();
        totalLength = 0.0f;
    }
} // namespace LaurelEye
