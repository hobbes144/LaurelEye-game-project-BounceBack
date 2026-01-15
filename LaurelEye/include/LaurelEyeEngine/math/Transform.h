/// @file   Transform.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @par    **Course**
///     CS529
/// @date   10-15-2024
/// @brief  Transform object, used to store the rotation, scale and translation
/// info of an object, along with providing operations on the same.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Quaternion.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye {

    class Transform {
    public:
        Transform() : position(0, 0, 0),
                      rotation(1, 0, 0, 0),
                      scaling(1, 1, 1) {}

        Transform(const Vector3& _position,
                  const Quaternion& _rotation,
                  const Vector3& _scale) : position(_position),
                                           rotation(_rotation),
                                           scaling(_scale) {}

        Transform(const Matrix4& matrix) : position(matrix.position()),
                                           rotation(matrix.toQuaternion()),
                                           scaling(matrix.scaling()) {};

        Transform operator*(const Transform& other) const;

        // Getters
        Vector3 getPosition() const;
        Quaternion getRotation() const;
        Vector3 getScaling() const;
        Matrix4 getLocalMatrix() const;
        Matrix4 getInverseLocalMatrix();
        Matrix4 getMatrix() const;
        Matrix4 getInverseMatrix();

        // Setters

        Transform& setPosition(const Vector3& newPosition);
        Transform& setPosition(float x, float y, float z);

        Transform& setRotation(const Vector3& newRotation);
        Transform& setRotation(float x, float y, float z);
        Transform& setRotation(const Quaternion& quaternion);

        Transform& setScaling(const Vector3& newScaling);
        Transform& setScaling(float x, float y, float z);

        // Operations
        Transform& translate(const Vector3& translation);
        Transform& translate(float x, float y, float z);

        Transform& rotate(const Quaternion& quaternionDelta);
        Transform& rotate(const Vector3& rotationDelta);
        Transform& rotate(float x, float y, float z);

        Transform& scale(const Vector3& scaleMult);
        Transform& scale(float x, float y, float z);

        // Interpolation
        static Transform lerp(const Transform& start, const Transform& end, float t);

    private:
        Vector3 position;
        Quaternion rotation;
        Vector3 scaling;
    };

} // namespace LaurelEye
