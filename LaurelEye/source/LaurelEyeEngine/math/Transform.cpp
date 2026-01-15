#include "LaurelEyeEngine/math/Transform.h"

namespace LaurelEye {

    Transform Transform::operator*(const Transform& other) const {
        Transform res;
        res.position = this->position + (this->rotation * (this->scaling * other.position));
        res.rotation = this->rotation * other.rotation;
        res.scaling = this->scaling * other.scaling;
        return res;
    }

    Vector3 Transform::getPosition() const {
        return position;
    }

    Quaternion Transform::getRotation() const {
        return rotation;
    }

    Vector3 Transform::getScaling() const {
        return scaling;
    }

    Transform& Transform::setPosition(const Vector3& newPosition) {
        position = newPosition;
        return *this;
    }

    Transform& Transform::setPosition(float x, float y, float z) {
        position = Vector3(x, y, z);
        return *this;
    }

    Transform& Transform::setRotation(const Vector3& newRotation) {
        rotation = Quaternion::fromEuler(newRotation);
        return *this;
    }

    Transform& Transform::setRotation(float x, float y, float z) {
        rotation = Quaternion::fromEuler(Vector3(x, y, z));
        return *this;
    }

    Transform& Transform::setRotation(const Quaternion& quaternion) {
        rotation = quaternion;
        return *this;
    }

    Transform& Transform::setScaling(const Vector3& newScaling) {
        scaling = newScaling;
        return *this;
    }

    Transform& Transform::setScaling(float x, float y, float z) {
        scaling = Vector3(x, y, z);
        return *this;
    }

    Matrix4 Transform::getLocalMatrix() const {
        return getMatrix();
    }

    Matrix4 Transform::getInverseLocalMatrix() {
        return getInverseMatrix();
    }

    Matrix4 Transform::getMatrix() const {
        return Matrix4::translation(position) *
               Matrix4::rotation(rotation) *
               Matrix4::scale(scaling);
    }

    Matrix4 Transform::getInverseMatrix() {
        Vector3 invTranslation(-position.x, -position.y, -position.z);
        Quaternion invRotation = rotation.inverse();
        Vector3 invScaling(1.0f / scaling.x, 1.0f / scaling.y, 1.0f / scaling.z);

        return Matrix4::scale(invScaling) *
               Matrix4::rotation(invRotation) *
               Matrix4::translation(invTranslation);
    }

    Transform& Transform::translate(const Vector3& translation) {
        position = position + translation;
        return *this;
    }

    Transform& Transform::translate(float x, float y, float z) {
        position = position + Vector3(x, y, z);
        return *this;
    }

    Transform& Transform::rotate(const Quaternion& quaternionDelta) {
        rotation = rotation * quaternionDelta;
        return *this;
    }

    Transform& Transform::rotate(const Vector3& rotationDelta) {
        Quaternion quaternionDelta = Quaternion::fromEuler(rotationDelta);
        rotation = rotation * quaternionDelta;
        return *this;
    }

    Transform& Transform::rotate(float x, float y, float z) {
        Quaternion quaternionDelta = Quaternion::fromEuler(Vector3(x, y, z));
        rotation = rotation * quaternionDelta;
        return *this;
    }

    Transform& Transform::scale(const Vector3& scaleMult) {
        scaling = scaling * scaleMult;
        return *this;
    }

    Transform& Transform::scale(float x, float y, float z) {
        scaling = scaling * Vector3(x, y, z);
        return *this;
    }

    Transform Transform::lerp(const Transform& start, const Transform& end, float t) {
        return {
            Vector3::lerp(start.position, end.position, t),
            Quaternion::slerp(start.rotation, end.rotation, t),
            Vector3::elerp(start.scaling, end.scaling, t)};
    }

} // namespace LaurelEye
