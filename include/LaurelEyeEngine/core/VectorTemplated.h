/*!****************************************************************************
 * \file   VectorTemplated.h
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
 *   (zaldivaralvarez@gmail.com) (Advising and Base Code)
 * \par    **Course**
 *    CS529
 * \date   10-14-2024
 * \brief  Templated Vector object for use with OpenGL functionality.
 *
 * Currently not in use. To be updated when ready to use.
 *
 * Copyright � 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#ifndef VECTOR_TEMPLATED_H
#define VECTOR_TEMPLATED_H

#pragma once

#include <array>
#include <cmath>
#include <stdexcept>

namespace LaurelEye {

template <typename T, size_t N>
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
        T result = 0;
        for ( int i = 0; i < N; i++ ) {
            result += pow(data[i], 2);
        }
        return result;
    }

    // Formula: sqrt(v1^2 + v2^2 + ... + vn^2)
    T magnitude() const {
        T result = 0;
        for ( int i = 0; i < N; i++ ) {
                result += static_cast<T>(pow(data[i], 2));
        }
        return sqrt(result);
    }

    // Formula: v / |v|, where |v| is the magnitude
    void normalize() {
        T magnitude_val = magnitude();
        if ( magnitude_val == 0 ) throw std::runtime_error("Cannot normalize zero vector");

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

    // Specific to 3D vectors
    // Formula: [v2*u3 - v3*u2, v3*u1 - v1*u3, v1*u2 - v2*u1]
    VectorTemplated cross(const VectorTemplated& other) const {
            return VectorTemplated((data[1] * other.data[2] - data[2] * other.data[1]), (data[2] * other.data[0] - data[0] * other.data[2]), (data[0] * other.data[1] - data[1] * other.data[0]));
    }
};

using Vector4 = VectorTemplated<float, 4>;

} // namespace LaurelEye

#endif // VECTOR_TEMPLATED_H
