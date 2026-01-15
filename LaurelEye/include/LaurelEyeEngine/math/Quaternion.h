/// @file   Quaternion.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   03-11-2025
/// @brief  Quaternion implementation for use with Transform
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include "LaurelEyeEngine/math/VectorTemplated.h"

#include <iostream>

namespace LaurelEye {

    class Vector3;

    template <typename T = float>
    class QuaternionBase : public VectorTemplated<T, 4>{
    private:
        bool isNormalized = false;
        bool isEulerVector = false;

        VectorTemplated<T, 3> eulerVector;

        void setDirty();

    public:
        QuaternionBase(T w = T(1.0), T x = T(.0),
                   T y = T(.0), T z = T(.0)) : VectorTemplated<T, 4>(w, x, y, z),
                                                   eulerVector(VectorTemplated<T, 3>()) {}

        QuaternionBase(VectorTemplated<T, 4> vec) : VectorTemplated<T, 4>(vec),
                                  eulerVector(VectorTemplated<T, 3>()) {
        }

        // Getter functions for w, x, y, z
        inline T w() const { return this->data[0]; }
        inline T x() const { return this->data[1]; }
        inline T y() const { return this->data[2]; }
        inline T z() const { return this->data[3]; }

        // Setter functions for w, x, y, z
        inline void setW(T w) {
            setDirty();
            this->data[0] = w;
        }
        inline void setX(T x) {
            setDirty();
            this->data[1] = x;
        }
        inline void setY(T y) {
            setDirty();
            this->data[2] = y;
        }
        inline void setZ(T z) { this->data[3] = z; }

        // Computes the forward vector
        VectorTemplated<T, 3> forward();

        // Computes the right vector
        VectorTemplated<T, 3> right();

        // Computes the up vector
        VectorTemplated<T, 3> up();

        QuaternionBase operator-() const;

        friend QuaternionBase operator+(QuaternionBase q, const QuaternionBase& other) {
            return q += other;
        }

        QuaternionBase operator+=(const QuaternionBase& other) {
            setDirty();
            VectorTemplated<T, 4>::operator+=(other);
            return *this;
        }

        friend QuaternionBase operator*(QuaternionBase q, const QuaternionBase& other) {
            return q *= other;
        }

        friend QuaternionBase operator*(QuaternionBase q, T other) {
            return q *= other;
        }

        QuaternionBase operator*=(const QuaternionBase& other) {
            setDirty();
            const QuaternionBase p(*this);
            this->data[0] = p.data[0] * other.data[0] - p.data[1] * other.data[1] - p.data[2] * other.data[2] - p.data[3] * other.data[3]; // w
            this->data[1] = p.data[0] * other.data[1] + p.data[1] * other.data[0] + p.data[2] * other.data[3] - p.data[3] * other.data[2]; // x
            this->data[2] = p.data[0] * other.data[2] - p.data[1] * other.data[3] + p.data[2] * other.data[0] + p.data[3] * other.data[1]; // y
            this->data[3] = p.data[0] * other.data[3] + p.data[1] * other.data[2] - p.data[2] * other.data[1] + p.data[3] * other.data[0]; // z
            return *this;
        }

        VectorTemplated<T, 3> operator*(const VectorTemplated<T, 3>& v) const;
        Vector3 operator*(const Vector3& v) const;

        QuaternionBase operator/(const T& b) const;

        QuaternionBase inverse();
        QuaternionBase conjugate() const;
        void normalize();
        QuaternionBase normalized();

        // Euler utilities
        // Axis 0 = x, 1 = y, 2 = z
        VectorTemplated<T, 3> getAxis(const int axis) const;

        Vector3 toEuler();
        static QuaternionBase fromEuler(const VectorTemplated<T, 3>& euler);
        static QuaternionBase fromEuler(const Vector3& euler);
        static QuaternionBase fromEuler(const T pitch, const T yaw, const T roll);
        static QuaternionBase axisAngleToQuaternion(const VectorTemplated<T, 3>& axis, T angle);

        static QuaternionBase slerp(const QuaternionBase& start, const QuaternionBase& end, T t);
        static QuaternionBase lerp(const QuaternionBase& start, const QuaternionBase& end, T t);
        static QuaternionBase elerp(const QuaternionBase& start, const QuaternionBase& end, T t) = delete;
    };

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const QuaternionBase<T>& q) {

        os << "[ ";
        os << q.w() << " " << q.x() << " " << q.y() << " " << q.z();
        os << " ]";

        return os;
    }

    extern template class QuaternionBase<float>;
    extern template class QuaternionBase<double>;
    using Quaternion = QuaternionBase<float>;
    using FQuaternion = QuaternionBase<float>;
    using DQuaternion = QuaternionBase<double>;

} // namespace LaurelEye
