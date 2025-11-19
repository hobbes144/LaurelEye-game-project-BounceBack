/// @file   VectorTemplated.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @par    **Course**
///     CS529
/// @date   10-14-2024
/// @brief  Templated Vector object for use with OpenGL functionality.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include <array>
#include <cstddef>

namespace LaurelEye {

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    class VectorTemplated {
    protected:
        std::array<T, N> data;

    public:
        VectorTemplated();
        VectorTemplated(const T* arr);
        VectorTemplated(const T val);
        VectorTemplated(const T val1, const T val2)
            requires(N == 2);
        VectorTemplated(const T val1, const T val2, const T val3)
            requires(N == 3);
        VectorTemplated(const T val1, const T val2, const T val3, const T val4)
            requires(N == 4);
        explicit VectorTemplated(const std::array<T, N>& values);

        VectorTemplated(const VectorTemplated&) = default;
        VectorTemplated& operator=(const VectorTemplated& other) = default;

        T& operator[](size_t index);
        const T& operator[](size_t index) const;

        bool operator==(const VectorTemplated& other) const;

        bool operator!=(const VectorTemplated& other) const;

        // Formula: [v1 + u1, v2 + u2, ..., vn + un]
        VectorTemplated& operator+=(const VectorTemplated& other);
        friend VectorTemplated operator+(VectorTemplated vector, const VectorTemplated& other) {
            return vector += other;
        }

        // Formula: [v1 - u1, v2 - u2, ..., vn - un]
        VectorTemplated operator-(const VectorTemplated& other) const;

        // Formula: [v1 * s, v2 * s, ..., vn * s]
        VectorTemplated operator*=(T scalar) const;
        friend VectorTemplated operator*(VectorTemplated vector, const VectorTemplated& other) {
            return vector *= other;
        }

        VectorTemplated& operator*=(const VectorTemplated& other);

        // Formula: v1*u1 + v2*u2 + ... + vn*un
        T dot(const VectorTemplated& other) const;

        // Formula: v1*u1 + v2*u2 + ... + vn*un
        VectorTemplated cross(const VectorTemplated& other) const
            requires(N == 3);

        // Formula: sqrt(v1^2 + v2^2 + ... + vn^2)
        T magnitudeSquared() const;
        T magnitude() const;

        // Formula: v / |v|, where |v| is the magnitude
        void normalize();

        // Formula: v / |v|, where |v| is the magnitude
        VectorTemplated normalized() const;

        static VectorTemplated lerp(const VectorTemplated& start, const VectorTemplated& end, T t);

        static VectorTemplated elerp(const VectorTemplated& start, const VectorTemplated& end, T t)
            requires std::floating_point<T>;
        static VectorTemplated elerp(const VectorTemplated& start, const VectorTemplated& end, T t)
            requires std::integral<T>;
    };

    // Using extern to force only one instantiation.
    // Reduces header compile size and time a tiny bit.
    extern template class VectorTemplated<int, 2ul>;
    extern template class VectorTemplated<float, 2ul>;
    extern template class VectorTemplated<double, 2ul>;
    extern template class VectorTemplated<int, 3ul>;
    extern template class VectorTemplated<float, 3ul>;
    extern template class VectorTemplated<double, 3ul>;
    extern template class VectorTemplated<int, 4ul>;
    extern template class VectorTemplated<float, 4ul>;
    extern template class VectorTemplated<double, 4ul>;
    using IVector2 = VectorTemplated<int, 2ul>;
    using Vector2 = VectorTemplated<float, 2ul>;
    using FVector2 = VectorTemplated<float, 2ul>;
    using DVector2 = VectorTemplated<double, 2ul>;
    using IVector3 = VectorTemplated<int, 3ul>;
    using FVector3 = VectorTemplated<float, 3ul>;
    using DVector3 = VectorTemplated<double, 3ul>;
    using IVector4 = VectorTemplated<int, 4ul>;
    using Vector4 = VectorTemplated<float, 4ul>;
    using FVector4 = VectorTemplated<float, 4ul>;
    using DVector4 = VectorTemplated<double, 4ul>;
} // namespace LaurelEye
