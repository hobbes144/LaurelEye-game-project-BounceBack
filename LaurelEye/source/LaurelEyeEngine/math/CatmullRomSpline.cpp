/// @file   CatmullRomSpline.cpp
/// @author Martin Chow (chowmartin387@gmail.com)
/// @par    **DigiPen Email**
///     martin.chow@digipen.edu
/// @par    **Course**
///     GAM541
/// @date   01-14-2026
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#include "LaurelEyeEngine/math/CatmullRomSpline.h"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

// #pragma message("Compiling CatmullRomSpline.cpp")

namespace LaurelEye {

    // Evaluate position on the spline at parameter t ∈ [0, 1].
    // this is mostly from https://www.mvps.org/directx/articles/catmull/
    Vector2 CatmullRomSpline::evaluate(float t) const {
        // Clamp t
        t = std::clamp(t, 0.0f, 1.0f);

        // Convert global t → segment index + local t
        int segCount = controlPoints.size() - 3; // Catmull–Rom uses 4 points per segment
        float scaled = t * segCount;

        int segIndex = std::min(int(scaled), segCount - 1);
        float localT = scaled - segIndex;

        // Evaluate that segment
        return evalSegment(segIndex + 1, localT);
    }

    // Evaluate derivative (tangent) at parameter t.
    // this is mostly from https://www.mvps.org/directx/articles/catmull/
    Vector2 CatmullRomSpline::derivative(float t) const {
// #pragma message("CatmullRomSpline::derivative is being compiled")
        t = std::clamp(t, 0.0f, 1.0f);
        int segCount = controlPoints.size() - 3;
        float scaled = t * segCount;
        int segIndex = std::min(int(scaled), segCount - 1);
        float localT = scaled - segIndex;
        return derivSegment(segIndex + 1, localT);
    }

    // this is basiclly just save the points and tension
    void CatmullRomSpline::toJSON(const std::string& filename) const {
        rapidjson::Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();

        // Type
        doc.AddMember("type", "CatmullRom", alloc);

        // Tension
        doc.AddMember("tension", tension, alloc);

        // Control points
        rapidjson::Value arr(rapidjson::kArrayType);
        for ( const auto& p : controlPoints ) {
            rapidjson::Value obj(rapidjson::kObjectType);
            obj.AddMember("x", p[0], alloc);
            obj.AddMember("y", p[1], alloc);
            arr.PushBack(obj, alloc);
        }
        doc.AddMember("controlPoints", arr, alloc);

        // Convert to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);

        // Write to file
        std::ofstream file(filename);
        if ( !file.is_open() ) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }

        file << buffer.GetString();
    }

    void CatmullRomSpline::fromJSON(const std::string& filename) {
        // Read file into string
        std::ifstream file(filename);
        if ( !file.is_open() ) {
            throw std::runtime_error("Failed to open file for reading: " + filename);
        }

        std::string json((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

        // Parse JSON
        rapidjson::Document doc;
        doc.Parse(json.c_str());

        if ( !doc.IsObject() ) {
            throw std::runtime_error("Invalid JSON in file: " + filename);
        }

        // Load tension
        if ( doc.HasMember("tension") )
            tension = doc["tension"].GetFloat();

        // Load control points
        controlPoints.clear();
        const auto& arr = doc["controlPoints"];
        for ( auto& v : arr.GetArray() ) {
            float x = v["x"].GetFloat();
            float y = v["y"].GetFloat();
            controlPoints.emplace_back(x, y);
        }

        arcLengthTable.buildIfNeeded(*this);
    }

    Vector2 CatmullRomSpline::evalSegment(int i, float t) const {
        // Catmull–Rom uses P[i-1], P[i], P[i+1], P[i+2]
        const Vector2& P0 = controlPoints[i - 1];
        const Vector2& P1 = controlPoints[i];
        const Vector2& P2 = controlPoints[i + 1];
        const Vector2& P3 = controlPoints[i + 2];

        float tau = tension; // 0.5f for centripetal

        // Tangents
        Vector2 T1 = (1.0f - tau) * 0.5f * (P2 - P0);
        Vector2 T2 = (1.0f - tau) * 0.5f * (P3 - P1);

        float t2 = t * t;
        float t3 = t2 * t;

        // Hermite basis
        float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
        float h10 = t3 - 2.0f * t2 + t;
        float h01 = -2.0f * t3 + 3.0f * t2;
        float h11 = t3 - t2;

        return h00 * P1 + h10 * T1 + h01 * P2 + h11 * T2;
    }

    Vector2 CatmullRomSpline::derivSegment(int i, float t) const {
        const Vector2& P0 = controlPoints[i - 1];
        const Vector2& P1 = controlPoints[i];
        const Vector2& P2 = controlPoints[i + 1];
        const Vector2& P3 = controlPoints[i + 2];

        float tau = tension;

        Vector2 T1 = (1.0f - tau) * 0.5f * (P2 - P0);
        Vector2 T2 = (1.0f - tau) * 0.5f * (P3 - P1);

        float t2 = t * t;

        // Derivatives of Hermite basis
        float h00 = 6.0f * t2 - 6.0f * t;
        float h10 = 3.0f * t2 - 4.0f * t + 1.0f;
        float h01 = -6.0f * t2 + 6.0f * t;
        float h11 = 3.0f * t2 - 2.0f * t;

        return h00 * P1 + h10 * T1 + h01 * P2 + h11 * T2;
    }

    // idk if this is used anywhere
    // just put it for extensibility
    void CatmullRomSpline::mapTtoSegment(float t, int& segIndex, float& localT) const {
        t = std::clamp(t, 0.0f, 1.0f);

        int segCount = static_cast<int>(controlPoints.size()) - 3;
        float scaled = t * segCount;

        segIndex = static_cast<int>(scaled);
        if ( segIndex >= segCount )
            segIndex = segCount - 1;

        localT = scaled - segIndex;
    }

    void CatmullRomSpline::update() {
        arcLengthTable.buildIfNeeded(*this);
    }


}
