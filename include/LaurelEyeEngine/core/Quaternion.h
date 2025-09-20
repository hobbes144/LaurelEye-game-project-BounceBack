/*!****************************************************************************
 * \file   Quaternion.h
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \date   03-11-2025
 * \brief  Quaternion implementation for use with Transform
 *
 * Copyright � 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#ifndef QUATERNION_H
#define QUATERNION_H

#pragma once

#include "LaurelEyeEngine/core/Vector3.h"
#include "LaurelEyeEngine/core/VectorTemplated.h"

namespace LaurelEye {

class Quaternion : public VectorTemplated<float, 4> {
private:
    bool isNormalized = false;
    bool isEulerVector = false;
    //bool isRotationMatrix = false;

    Vector3 eulerVector;
    //Matrix4 rotationMatrix;

    void setDirty();

public:
        Quaternion(float w = 1.0f, float x = .0f, float y = .0f, float z = .0f) : VectorTemplated(), eulerVector(Vector3()) { //, rotationMatrix(Matrix4()) {
            data[0] = w;                                                                                                      // w
            data[1] = x;                                                                                                      // x
            data[2] = y;                                                                                                      // y
            data[3] = z;                                                                                                      // z
    }

    Quaternion(VectorTemplated<float, 4> vec) : VectorTemplated(vec),
        eulerVector(Vector3()) {
        }

// Getter functions for w, x, y, z
    inline float w() const { return data[0]; }
    inline float x() const { return data[1]; }
    inline float y() const { return data[2]; }
    inline float z() const { return data[3]; }

    // Setter functions for w, x, y, z
        inline void setW(float w) {
            setDirty();
            data[0] = w;
        }
        inline void setX(float x) {
            setDirty();
            data[1] = x;
        }
        inline void setY(float y) {
            setDirty();
            data[2] = y;
        }
    inline void setZ(float z) { data[3] = z; }

    Vector3 forward();

    // Computes the right vector
    Vector3 right();

    // Computes the up vector
    Vector3 up();

    //friend Quaternion operator*(const Matrix4&, const Quaternion&);

    friend Quaternion operator+(Quaternion q, const Quaternion& other) {
        return q += other;
    }

    Quaternion operator+=(const Quaternion& other) {
        setDirty();
        VectorTemplated::operator+=(other);
        return *this;
    }

    friend Quaternion operator*(Quaternion q, const Quaternion& other) {
        return q *= other;
    }

    Quaternion operator*=(const Quaternion& other) {
        setDirty();
        const Quaternion p(*this);
            data[0] = p.data[0] * other.data[0] - p.data[1] * other.data[1] - p.data[2] * other.data[2] - p.data[3] * other.data[3]; // w
            data[1] = p.data[0] * other.data[1] + p.data[1] * other.data[0] + p.data[2] * other.data[3] - p.data[3] * other.data[2]; // x
            data[2] = p.data[0] * other.data[2] - p.data[1] * other.data[3] + p.data[2] * other.data[0] + p.data[3] * other.data[1]; // y
            data[3] = p.data[0] * other.data[3] + p.data[1] * other.data[2] - p.data[2] * other.data[1] + p.data[3] * other.data[0]; // z
        return *this;
    }

    Vector3 operator*(const Vector3& v) const {
        const Vector3 quatVec(data[1], data[2], data[3]);

        const Vector3 uv = quatVec.cross(v);
        const Vector3 uuv = quatVec.cross(uv);
            return v + ((uv * data[0]) + uuv) * 2.0f;
    }

    Quaternion inverse();
    Quaternion conjugate() const;
    void normalize();
    Quaternion normalized();

    // Euler utilities
    // Axis 0 = x, 1 = y, 2 = z
    Vector3 getAxis(const int axis) const;
    Vector3 toEuler();
    static Quaternion fromEuler(const Vector3& euler);
    static Quaternion fromEuler(const float pitch, const float yaw, const float roll);
    static Quaternion axisAngleToQuaternion(const Vector3& axis, float angle);
};

std::ostream& operator<<(std::ostream& os, const Quaternion& q);
} // namespace LaurelEye

#endif // !QUATERNION_H
