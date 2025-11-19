/// @file   Vector3.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @par    **Course**
///     GAM541
/// @date   10-05-2024
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#include "LaurelEyeEngine/math/Vector3.h"

#include <cmath>
#include <stdexcept>

namespace LaurelEye {

    Vector3 Vector3::operator+(const Vector3& other) const {
        Vector3 result = Vector3(*this);
        return result += other;
    }

    Vector3 Vector3::operator+(float scalar) const {
        Vector3 result = Vector3(*this);
        return result += scalar;
    }

    Vector3& Vector3::operator+=(const Vector3& other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    Vector3& Vector3::operator+=(float scalar) {
        this->x += scalar;
        this->y += scalar;
        this->z += scalar;
        return *this;
    }

    Vector3 Vector3::operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 Vector3::operator-(float scalar) const {
        return Vector3(x - scalar, y - scalar, z - scalar);
    }

    Vector3 Vector3::operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 Vector3::operator*(const Vector3& other) const {
        Vector3 result = Vector3(*this);
        return result *= other;
    }

    Vector3 Vector3::operator*(float scalar) const {
        Vector3 result = Vector3(*this);
        return result *= scalar;
    }

    Vector3& Vector3::operator*=(const Vector3& other) {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;
        return *this;
    }

    Vector3& Vector3::operator*=(float scalar) {
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;
        return *this;
    }

    Vector3 Vector3::operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    Vector3 Vector3::operator/(const Vector3& other) const {
        return Vector3(x / other.x, y / other.y, z / other.z);
    }

    bool Vector3::operator==(const Vector3& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    bool Vector3::operator!=(const Vector3& other) const {
        return (!(*this == other));
    }

    float Vector3::operator[](int index) const {
        if ( index == 0 ) return x;
        else if ( index == 1 ) return y;
        else return z;
    }

    float& Vector3::operator[](const int index) {
        if ( index == 0 ) return x;
        else if ( index == 1 ) return y;
        else return z;
    }

    float Vector3::dot(const Vector3& other) const {
        return ((x * other.x) + (y * other.y) + (z * other.z));
    }

    float Vector3::magnitude() const {
        return (float)(sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));
    }

    float Vector3::magnitudSquared() const {
        return (float)(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    Vector3 Vector3::normalized() const {
        float magnitude_val = magnitude();
        if ( magnitude_val == 0 ) throw std::runtime_error("Cannot normalize zero vector");
        return Vector3(x / magnitude_val, y / magnitude_val, z / magnitude_val);
    }

    Vector3 Vector3::cross(const Vector3& other) const {
        return Vector3((y * other.z - z * other.y), (z * other.x - x * other.z), (x * other.y - y * other.x));
    }

    Vector3 Vector3::reciprocal() const {
        const float largeNumber = 1e30f;
        return Vector3(
            x != 0.0f ? 1.0f / x : largeNumber,
            y != 0.0f ? 1.0f / y : largeNumber,
            z != 0.0f ? 1.0f / z : largeNumber);
    }

    Vector3 Vector3::project(const Vector3& other) const {
        return (
            other * (this->dot(other) / other.magnitudSquared()));
    }

    Vector3 Vector3::abs() const {
        return Vector3(fabs(x), fabs(y), fabs(z));
    }

    int Vector3::getMaxIndex() const {
        return (x >= y) ? ((x >= z) ? 0 : 2) : (y >= z) ? 1
                                                        : 2;
    }

    // Friend functions

    Vector3 operator+(float scalar, const Vector3& other) {
        return other + scalar;
    }

    Vector3 operator-(float scalar, const Vector3& other) {
        return other - scalar;
    }

    Vector3 operator*(float scalar, const Vector3& other) {
        return other * scalar;
    }

    bool operator<(const Vector3& first, const Vector3& other) {
        if ( first.magnitudSquared() < other.magnitudSquared() ) return true;
        return false;
    }

    bool operator>(const Vector3& first, const Vector3& other) {
        return other < first;
    }

    bool operator<=(const Vector3& first, const Vector3& other) {
        return !(other < first);
    }

    bool operator>=(const Vector3& first, const Vector3& other) {
        return !(first < other);
    }

    Vector3 Vector3::lerp(const Vector3& start, const Vector3& end, float t) {
        return Vector3((1.0f - t) * start + t * end);
    }

    Vector3 Vector3::elerp(const Vector3& start, const Vector3& end, float t) {
        return Vector3(
            start.x*pow(end.x/start.x, t),
            start.y*pow(end.y/start.y, t),
            start.z*pow(end.z/start.z, t));
    }

    std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << std::fixed << std::setprecision(2) << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

} // namespace LaurelEye
