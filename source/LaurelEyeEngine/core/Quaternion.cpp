/*!****************************************************************************
 * \file   Quaternion.cpp
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   04-18-2025
 *
 * Copyright � 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#include "LaurelEyeEngine/core/Quaternion.h"

namespace LaurelEye {

    void Quaternion::setDirty() {
        isNormalized = false;
        isEulerVector = false;
        // isRotationMatrix = false;
    }

    Vector3 Quaternion::forward() {
        normalize();

        return Vector3(
            2.0f * (data[1] * data[3] + data[0] * data[2]),
            2.0f * (data[2] * data[3] - data[0] * data[1]),
            1.0f - 2.0f * (data[1] * data[1] + data[2] * data[2]));
    }

    Vector3 Quaternion::right() {
        normalize();

        return Vector3(
            1.0f - 2.0f * (data[2] * data[2] + data[3] * data[3]),
            2.0f * (data[1] * data[2] + data[0] * data[3]),
            2.0f * (data[1] * data[3] - data[0] * data[2]));
    }

    Vector3 Quaternion::up() {
        normalize();

        return Vector3(
            2.0f * (data[1] * data[2] - data[0] * data[3]),
            1.0f - 2.0f * (data[1] * data[1] + data[3] * data[3]),
            2.0f * (data[2] * data[3] + data[0] * data[1]));
    }

    // Axis 0 = x, 1 = y, 2 = z

    Quaternion Quaternion::inverse() {
        normalize();
        return conjugate();
    }

    Quaternion Quaternion::conjugate() const {
        Quaternion result = Quaternion(data[0], -data[1], -data[2], -data[3]);
        result.isNormalized = isNormalized;
        return result;
    }

    void Quaternion::normalize() {
        if ( !isNormalized ) {
            VectorTemplated::normalize();
            isNormalized = true;
        }
    }

    Quaternion Quaternion::normalized() {
        normalize();
        Quaternion result = *this;
        return result;
    }

    Vector3 Quaternion::getAxis(const int axis) const {
        switch ( axis ) {
        case 0:
            return *this * Vector3(1, 0, 0);
            break;
        case 1:
            return *this * Vector3(0, 1, 0);
            break;
        case 2:
            return *this * Vector3(0, 0, 1);
            break;
        default:
            assert(false && "Invalid axis provided");
            return Vector3(0.0f);
        }
    }

    Vector3 Quaternion::toEuler() {
        if ( isEulerVector )
            return eulerVector;
        normalize();

        float y, x;

        // Note: following code was taken from glm
        // Roll (X-axis rotation)
        y = 2.0f * (data[1] * data[2] + data[0] * data[3]);
        x = data[0] * data[0] + data[1] * data[1] - data[2] * data[2] - data[3] * data[3];

        if ( x == 0 && y == 0 ) // avoid atan2(0,0) - handle singularity - Matiis
            eulerVector.z = 0.0f;
        else
            eulerVector.z = atan2(y, x);

        // Pitch (Y-axis rotation)
        y = 2.0f * (data[2] * data[3] + data[0] * data[1]);
        x = data[0] * data[0] - data[1] * data[1] - data[2] * data[2] + data[3] * data[3];
        if ( x == 0 && y == 0 ) // avoid atan2(0,0) - handle singularity - Matiis
            eulerVector.x = 2.0f * atan2(data[1], data[0]);
        else
            eulerVector.x = atan2(y, x);

        // Yaw (Z-axis rotation)
        eulerVector.y = asin(std::min(std::max(-2.0f * (data[1] * data[3] - data[0] * data[2]), -1.0f), 1.0f));

        isEulerVector = true;
        return eulerVector;
    }

    Quaternion Quaternion::fromEuler(const Vector3& euler) {
        float cx = cos(euler.x * 0.5f);
        float sx = sin(euler.x * 0.5f);
        float cy = cos(euler.y * 0.5f);
        float sy = sin(euler.y * 0.5f);
        float cz = cos(euler.z * 0.5f);
        float sz = sin(euler.z * 0.5f);

        Quaternion q;
        q[0] = cx * cy * cz + sx * sy * sz; // w
        q[1] = sx * cy * cz - cx * sy * sz; // x
        q[2] = cx * sy * cz + sx * cy * sz; // y
        q[3] = cx * cy * sz - sx * sy * cz; // z

        return q;
    }

    Quaternion Quaternion::fromEuler(const float pitch, const float yaw, const float roll) {
        float cx = cos(pitch * 0.5f);
        float sx = sin(pitch * 0.5f);
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cz = cos(roll * 0.5f);
        float sz = sin(roll * 0.5f);

        Quaternion q;
        q[0] = cx * cy * cz + sx * sy * sz; // w
        q[1] = sx * cy * cz - cx * sy * sz; // x
        q[2] = cx * sy * cz + sx * cy * sz; // y
        q[3] = cx * cy * sz - sx * sy * cz; // z

        return q;
    }

    Quaternion Quaternion::axisAngleToQuaternion(const Vector3& axis, float angle) {
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        return Quaternion(std::cos(halfAngle), axis.x * s, axis.y * s, axis.z * s);
    }

    std::ostream& operator<<(std::ostream& os, const Quaternion& q) {

        os << "[ ";
        os << q.w() << " " << q.x() << " " << q.y() << " " << q.z();
        os << " ]";

        return os;
    }

} // namespace LaurelEye
