/// @file   Vector3.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @par    **Course**
///     CS529
/// @date   10-14-2024
/// @brief  File conatining the custom Vector3 implementation for use with
/// OpenGL functions and other game logic.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include "LaurelEyeEngine/math/VectorTemplated.h"

#include <concepts>
#include <iostream>
#include <vector>

namespace LaurelEye {

    constexpr float PI{3.14159265f};

    class Vector3 {
    public:
        float x, y, z;

        // Default Constructor
        Vector3() : x(0), y(0), z(0) {}

        // Parameterized Constructor with Initialization List
        template <std::convertible_to<float> Numeric>
        Vector3(Numeric x, Numeric y, Numeric z) : x(float(x)), y(float(y)), z(float(z)) {}

        template <std::convertible_to<float> Numeric>
        Vector3(Numeric val) : x(float(val)), y(float(val)), z(float(val)) {}

        template <std::convertible_to<float> Numeric>
        Vector3(VectorTemplated<Numeric, 3> vec) : x(float(vec[0])), y(float(vec[1])), z(float(vec[2])) {}

        operator VectorTemplated<float, 3>() const { return VectorTemplated<float, 3>(x, y, z); }

        // Operator overloads
        Vector3 operator+(const Vector3& other) const;
        Vector3 operator+(float scalar) const;
        Vector3& operator+=(const Vector3& other);
        Vector3& operator+=(float scalar);
        friend Vector3 operator+(float scalar, const Vector3& other);
        Vector3 operator-(const Vector3& other) const;
        Vector3 operator-(float scalar) const;
        friend Vector3 operator-(float scalar, const Vector3& other);
        Vector3 operator-() const;
        Vector3 operator*(float scalar) const;
        Vector3 operator*(const Vector3& other) const;
        Vector3& operator*=(const Vector3& other);
        Vector3& operator*=(float scalar);
        friend Vector3 operator*(float scalar, const Vector3& other);
        Vector3 operator/(float scalar) const;
        Vector3 operator/(const Vector3& other) const;
        bool operator==(const Vector3& other) const;
        bool operator!=(const Vector3& other) const;
        friend bool operator<(const Vector3&, const Vector3&);
        friend bool operator>(const Vector3&, const Vector3&);
        friend bool operator<=(const Vector3&, const Vector3&);
        friend bool operator>=(const Vector3&, const Vector3&);
        float operator[](int index) const;
        float& operator[](const int index);

        // Main mathematical operations
        float dot(const Vector3& other) const;
        float magnitude() const;
        inline float length() const { return magnitude(); };
        float magnitudSquared() const;
        Vector3 normalized() const;
        Vector3 cross(const Vector3& other) const;
        Vector3 reciprocal() const;
        Vector3 project(const Vector3& other) const;
        Vector3 abs() const;

        int getMaxIndex() const;

        std::vector<float> getData() const { return {x, y, z}; }

        static Vector3 lerp(const Vector3& start, const Vector3& end, float t);
        static Vector3 elerp(const Vector3& start, const Vector3& end, float t);

        friend std::ostream& operator<<(std::ostream& os, const Vector3& v);
    };

} // namespace LaurelEye
