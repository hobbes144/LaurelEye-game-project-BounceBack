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
        Transform() : position(0, 0, 0), rotation(1, 0, 0, 0), scaling(1, 1, 1) {}
        Transform(Vector3 _position, Quaternion _rotation, Vector3 _scale) : position(_position), rotation(_rotation), scaling(_scale) {}

        Transform operator*(const Transform& other) const {
            Transform res;
            res.position = this->position + (this->rotation * (this->scaling * other.position));
            res.rotation = this->rotation * other.rotation;
            res.scaling = this->scaling * other.scaling;
            return res;
        }

        Vector3 getPosition() const { return position; }
        Quaternion getRotation() const { return rotation; }
        Vector3 getScaling() const { return scaling; }

        Transform& setPosition(const Vector3& newPosition) {
            position = newPosition;
            return *this;
        }
        Transform& setPosition(float x, float y, float z) {
            position = Vector3(x, y, z);
            return *this;
        }
        Transform& setRotation(const Vector3& newRotation) {
            rotation = Quaternion::fromEuler(newRotation);
            return *this;
        }
        Transform& setRotation(float x, float y, float z) {
            rotation = Quaternion::fromEuler(Vector3(x, y, z));
            return *this;
        }
        Transform& setRotation(const Quaternion& quaternion) {
            rotation = quaternion;
            return *this;
        }
        Transform& setScaling(const Vector3& newScaling) {
            scaling = newScaling;
            return *this;
        }
        Transform& setScaling(float x, float y, float z) {
            scaling = Vector3(x, y, z);
            return *this;
        }

        Matrix4 getLocalMatrix() const {
            return Matrix4::translation(position) *
                   Matrix4::rotation(rotation) *
                   Matrix4::scale(scaling);
        }

        Matrix4 getInverseLocalMatrix() {
            Vector3 invTranslation(-position.x, -position.y, -position.z);
            Quaternion invRotation = rotation.inverse();
            Vector3 invScaling(1.0f / scaling.x, 1.0f / scaling.y, 1.0f / scaling.z);

            return Matrix4::scale(invScaling) *
                   Matrix4::rotation(invRotation) *
                   Matrix4::translation(invTranslation);
        }

        Transform& translate(const Vector3& translation) {
            position = position + translation;
            return *this;
        }

        Transform& translate(float x, float y, float z) {
            position = position + Vector3(x, y, z);
            return *this;
        }

        Transform& rotate(const Quaternion quaternionDelta) {
            rotation = rotation * quaternionDelta;
            return *this;
        }

        Transform& rotate(const Vector3 rotationDelta) {
            Quaternion quaternionDelta = Quaternion::fromEuler(rotationDelta);
            rotation = rotation * quaternionDelta;
            return *this;
        }

        Transform& rotate(float x, float y, float z) {
            Quaternion quaternionDelta = Quaternion::fromEuler(Vector3(x, y, z));
            rotation = rotation * quaternionDelta;
            return *this;
        }

        Transform& scale(const Vector3& scaleMult) {
            scaling = scaling * scaleMult;
            return *this;
        }

        Transform& scale(float x, float y, float z) {
            scaling = scaling * Vector3(x, y, z);
            return *this;
        }

    private:
        Vector3 position;
        Quaternion rotation;
        Vector3 scaling;
    };

} // namespace LaurelEye
