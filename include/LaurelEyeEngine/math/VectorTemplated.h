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
#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace LaurelEye {

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    class VectorTemplated {
    protected:
        std::array<T, N> data;

    public:
        VectorTemplated() : data{} {}
        VectorTemplated(const T* arr) : data{} {
            for ( int i = 0; i < N; ++i ) {
                data[i] = arr[i];
            }
        }

        VectorTemplated(const T val) : data{} {
            for ( int i = 0; i < N; ++i ) {
                data[i] = val;
            }
        }

        VectorTemplated(const T val1, const T val2)
            requires(N == 2)
            : data{val1, val2} {}
        VectorTemplated(const T val1, const T val2, const T val3)
            requires(N == 3)
            : data{val1, val2, val3} {}
        VectorTemplated(const T val1, const T val2, const T val3, const T val4)
            requires(N == 4)
            : data{val1, val2, val3, val4} {}

        VectorTemplated& operator=(const VectorTemplated& other) {
            if ( *this == other ) {
                return *this;
            }
            data = other.data;
            return *this;
        }

        explicit VectorTemplated(const std::array<T, N>& values) : data(values) {}

        T& operator[](size_t index) { return data[index]; }
        const T& operator[](size_t index) const { return data[index]; }

        bool operator==(const VectorTemplated& other) const {
            for ( int i = 0; i < N; ++i ) {
                if ( data[i] != other[i] ) return false;
            }
            return true;
        }

        bool operator!=(const VectorTemplated& other) const {
            return (!(*this == other));
        }

        // Formula: [v1 + u1, v2 + u2, ..., vn + un]
        friend VectorTemplated operator+(VectorTemplated vector, const VectorTemplated& other) {
            return vector += other;
        }

        VectorTemplated& operator+=(const VectorTemplated& other) {
            for ( int i = 0; i < N; i++ ) {
                data[i] += other.data[i];
            }

            return *this;
        }

        // Formula: [v1 - u1, v2 - u2, ..., vn - un]
        VectorTemplated operator-(const VectorTemplated& other) const {
            VectorTemplated result;
            for ( int i = 0; i < N; i++ ) {
                result.data[i] = data[i] - other.data[i];
            }

            return result;
        }

        // Formula: [v1 * s, v2 * s, ..., vn * s]
        VectorTemplated operator*(T scalar) const {
            VectorTemplated result;
            for ( int i = 0; i < N; i++ ) {
                result.data[i] = data[i] * scalar;
            }

            return result;
        }

        friend VectorTemplated operator*(VectorTemplated vector, const VectorTemplated& other) {
            return vector *= other;
        }

        VectorTemplated& operator*=(const VectorTemplated& other) {
            for ( int i = 0; i < N; i++ ) {
                data[i] *= other.data[i];
            }

            return *this;
        }

        // Formula: v1*u1 + v2*u2 + ... + vn*un
        T dot(const VectorTemplated& other) const {
            T result = 0;
            for ( int i = 0; i < N; i++ ) {
                result += data[i] * other.data[i];
            }

            return result;
        }

        T magnitudeSquared() const {
            T result = static_cast<T>(0);
            for ( int i = 0; i < N; i++ ) {
                result += static_cast<T>(pow(data[i], 2.0));
            }
            return result;
        }

        // Formula: sqrt(v1^2 + v2^2 + ... + vn^2)
        T magnitude() const {
            T result = 0;
            for ( int i = 0; i < N; i++ ) {
                result += static_cast<T>(pow(data[i], 2.0));
            }
            return static_cast<T>(sqrt(result));
        }

        // Formula: v / |v|, where |v| is the magnitude
        void normalize() {
            T magnitude_val = magnitude();
            if ( magnitude_val == static_cast<T>(0) ) throw std::runtime_error("Cannot normalize zero vector");

            for ( int i = 0; i < N; i++ ) {
                data[i] /= magnitude_val;
            }
        }

        // Formula: v / |v|, where |v| is the magnitude
        VectorTemplated normalized() const {
            VectorTemplated result = *this;
            result.normalize();
            return result;
        }
    };

    // Using extern to force only one instantiation.
    // Reduces header compile size and time a tiny bit.
    extern template class VectorTemplated<int, 2>;
    extern template class VectorTemplated<float, 2>;
    extern template class VectorTemplated<double, 2>;
    extern template class VectorTemplated<int, 3>;
    extern template class VectorTemplated<double, 3>;
    extern template class VectorTemplated<int, 4>;
    extern template class VectorTemplated<float, 4>;
    extern template class VectorTemplated<double, 4>;
    using IVector2 = VectorTemplated<int, 2>;
    using Vector2 = VectorTemplated<float, 2>;
    using FVector2 = VectorTemplated<float, 2>;
    using DVector2 = VectorTemplated<double, 2>;
    using IVector3 = VectorTemplated<int, 3>;
    using DVector3 = VectorTemplated<double, 3>;
    using IVector4 = VectorTemplated<int, 4>;
    using Vector4 = VectorTemplated<float, 4>;
    using FVector4 = VectorTemplated<float, 4>;
    using DVector4 = VectorTemplated<double, 4>;
} // namespace LaurelEye
