/// @file   QuaternionBase<T>.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @par    **Course**
///     GAM541
/// @date   04-18-2025
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#include "LaurelEyeEngine/math/Quaternion.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye {

    template <typename T>
    void QuaternionBase<T>::setDirty() {
        isNormalized = false;
        isEulerVector = false;
    }

    template <typename T>
    VectorTemplated<T, 3> QuaternionBase<T>::forward() {
        normalize();

        return VectorTemplated<T, 3>(
            2.0f * (this->data[1] * this->data[3] + this->data[0] * this->data[2]),
            2.0f * (this->data[2] * this->data[3] - this->data[0] * this->data[1]),
            1.0f - 2.0f * (this->data[1] * this->data[1] + this->data[2] * this->data[2]));
    }

    template <typename T>
    VectorTemplated<T, 3> QuaternionBase<T>::right() {
        normalize();

        return VectorTemplated<T, 3>(
            1.0f - 2.0f * (this->data[2] * this->data[2] + this->data[3] * this->data[3]),
            2.0f * (this->data[1] * this->data[2] + this->data[0] * this->data[3]),
            2.0f * (this->data[1] * this->data[3] - this->data[0] * this->data[2]));
    }

    template <typename T>
    VectorTemplated<T, 3> QuaternionBase<T>::up() {
        normalize();

        return VectorTemplated<T, 3>(
            2.0f * (this->data[1] * this->data[2] - this->data[0] * this->data[3]),
            1.0f - 2.0f * (this->data[1] * this->data[1] + this->data[3] * this->data[3]),
            2.0f * (this->data[2] * this->data[3] + this->data[0] * this->data[1]));
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::operator-() const {
        return QuaternionBase<T>(-this->data[0], -this->data[1], -this->data[2], -this->data[3]);
    }

    template <typename T>
    VectorTemplated<T, 3> QuaternionBase<T>::operator*(const VectorTemplated<T, 3>& v) const {
        const VectorTemplated<T, 3> quatVec(this->data[1], this->data[2], this->data[3]);

        const VectorTemplated<T, 3> uv = quatVec.cross(v);
        const VectorTemplated<T, 3> uuv = quatVec.cross(uv);
        return v + ((uv * this->data[0]) + uuv) * 2.0f;
    }

    template <typename T>
    Vector3 QuaternionBase<T>::operator*(const Vector3& v) const {
        const Vector3 quatVec(this->data[1], this->data[2], this->data[3]);

        const Vector3 uv = quatVec.cross(v);
        const Vector3 uuv = quatVec.cross(uv);
        return v + ((uv * this->data[0]) + uuv) * 2.0f;
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::operator/(const T& b) const {
        return QuaternionBase<T>(this->data[0] / b, this->data[1] / b, this->data[2] / b, this->data[3] / b);
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::inverse() {
        normalize();
        return conjugate();
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::conjugate() const {
        QuaternionBase<T> result = QuaternionBase<T>(this->data[0], -this->data[1], -this->data[2], -this->data[3]);
        result.isNormalized = isNormalized;
        return result;
    }

    template <typename T>
    void QuaternionBase<T>::normalize() {
        if ( !isNormalized ) {
            VectorTemplated<T, 4>::normalize();
            isNormalized = true;
        }
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::normalized() {
        normalize();
        QuaternionBase<T> result = *this;
        return result;
    }

    // Axis 0 = x, 1 = y, 2 = z
    template <typename T>
    VectorTemplated<T, 3> QuaternionBase<T>::getAxis(const int axis) const {
        switch ( axis ) {
        case 0:
            return *this * VectorTemplated<T, 3>(1, 0, 0);
            break;
        case 1:
            return *this * VectorTemplated<T, 3>(0, 1, 0);
            break;
        case 2:
            return *this * VectorTemplated<T, 3>(0, 0, 1);
            break;
        default:
            assert(false && "Invalid axis provided");
            return VectorTemplated<T, 3>(0.0f);
        }
    }

    template <typename T>
    Vector3 QuaternionBase<T>::toEuler() {
        if ( isEulerVector )
            return Vector3(eulerVector);
        normalize();

        float y, x;

        // Note: following code was taken from glm
        // Roll (X-axis rotation)
        y = 2.0f * (this->data[1] * this->data[2] + this->data[0] * this->data[3]);
        x = this->data[0] * this->data[0] + this->data[1] * this->data[1] -
            this->data[2] * this->data[2] - this->data[3] * this->data[3];

        if ( x == 0 && y == 0 ) // avoid atan2(0,0) - handle singularity - Matiis
            eulerVector[2] = 0.0f;
        else
            eulerVector[2] = atan2(y, x);

        // Pitch (Y-axis rotation)
        y = 2.0f * (this->data[2] * this->data[3] + this->data[0] * this->data[1]);
        x = this->data[0] * this->data[0] - this->data[1] * this->data[1] -
            this->data[2] * this->data[2] + this->data[3] * this->data[3];
        if ( x == 0 && y == 0 ) // avoid atan2(0,0) - handle singularity - Matiis
            eulerVector[0] = 2.0f * atan2(this->data[1], this->data[0]);
        else
            eulerVector[0] = atan2(y, x);

        // Yaw (Z-axis rotation)
        eulerVector[1] = asin(std::min(
            std::max(
                T(-2.0f) * (this->data[1] * this->data[3] - this->data[0] * this->data[2]),
                T(-1.0f)),
            T(1.0f)));

        isEulerVector = true;
        return Vector3(eulerVector);
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::fromEuler(const VectorTemplated<T, 3>& euler) {
        float cx = cos(euler[0] * 0.5f);
        float sx = sin(euler[0] * 0.5f);
        float cy = cos(euler[1] * 0.5f);
        float sy = sin(euler[1] * 0.5f);
        float cz = cos(euler[2] * 0.5f);
        float sz = sin(euler[2] * 0.5f);

        QuaternionBase<T> q;
        q[0] = cx * cy * cz + sx * sy * sz; // w
        q[1] = sx * cy * cz - cx * sy * sz; // x
        q[2] = cx * sy * cz + sx * cy * sz; // y
        q[3] = cx * cy * sz - sx * sy * cz; // z

        return q;
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::fromEuler(const Vector3& euler) {
        float cx = cos(euler.x * 0.5f);
        float sx = sin(euler.x * 0.5f);
        float cy = cos(euler.y * 0.5f);
        float sy = sin(euler.y * 0.5f);
        float cz = cos(euler.z * 0.5f);
        float sz = sin(euler.z * 0.5f);

        QuaternionBase<T> q;
        q[0] = cx * cy * cz + sx * sy * sz; // w
        q[1] = sx * cy * cz - cx * sy * sz; // x
        q[2] = cx * sy * cz + sx * cy * sz; // y
        q[3] = cx * cy * sz - sx * sy * cz; // z

        return q;
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::fromEuler(const T pitch, const T yaw, const T roll) {
        float cx = cos(pitch * 0.5f);
        float sx = sin(pitch * 0.5f);
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cz = cos(roll * 0.5f);
        float sz = sin(roll * 0.5f);

        QuaternionBase<T> q;
        q[0] = cx * cy * cz + sx * sy * sz; // w
        q[1] = sx * cy * cz - cx * sy * sz; // x
        q[2] = cx * sy * cz + sx * cy * sz; // y
        q[3] = cx * cy * sz - sx * sy * cz; // z

        return q;
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::axisAngleToQuaternion(const VectorTemplated<T, 3>& axis, T angle) {
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        return QuaternionBase<T>(std::cos(halfAngle), axis[0] * s, axis[1] * s, axis[2] * s);
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::slerp(const QuaternionBase<T>& x, const QuaternionBase<T>& y, T a) {
        QuaternionBase<T> z = y;

        float cosTheta = x.dot(y);

        // If cosTheta < 0, the interpolation will take the long way around the sphere.
        // To fix this, one quat must be negated.
        if ( cosTheta < static_cast<T>(0) ) {
            z = -y;
            cosTheta = -cosTheta;
        }

        // Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
        if ( cosTheta > static_cast<T>(1) - std::numeric_limits<T>::epsilon() ) {
            // Linear interpolation
            return QuaternionBase<T>(
                (1 - a) * x[0] + a * z[0],
                (1 - a) * x[1] + a * z[1],
                (1 - a) * x[2] + a * z[2],
                (1 - a) * x[3] + a * z[3]);
        }
        else {
            // Essential Mathematics, page 467
            float angle = acos(cosTheta);
            return (sin((static_cast<T>(1) - a) * angle) * x + sin(a * angle) * z) / sin(angle);
        }
    }

    template <typename T>
    QuaternionBase<T> QuaternionBase<T>::lerp(const QuaternionBase<T>& start, const QuaternionBase<T>& end, T t) {
        return start * ((1.0f) - t) + (end * t);
    }

    template class QuaternionBase<float>;
    template class QuaternionBase<double>;

} // namespace LaurelEye
