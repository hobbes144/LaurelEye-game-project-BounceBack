#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math.h"

namespace LaurelEye::Scripting {
    void Sol2API_Math::setupMatrix4(sol::state& lua) {
        lua.new_usertype<Matrix4>("Matrix4",
            // Constructors
            sol::constructors<
                Matrix4(),
                Matrix4(float,float,float,float,
                        float,float,float,float,
                        float,float,float,float,
                        float,float,float,float)
            >(),

            // Operator Overloads
            sol::meta_function::multiplication, sol::overload(
                [](const Matrix4& m, float scalar) { return m * scalar; },
                [](const Matrix4& m, const Matrix4& other) {
                    Matrix4 copy = m;
                    return copy * other;
                },
                [](const Matrix4& m, const Vector3& v) { return m * v; }
            ),

            sol::meta_function::index, [](Matrix4& m, int row) -> float* {
                return m[row];
            },

            // Instance Methods
            "getElement", &Matrix4::getElement,
            "updateElement", &Matrix4::updateElement,
            "getData", &Matrix4::getData,
            "transformDirection", &Matrix4::transformDirection,
            "toQuaternion", &Matrix4::toQuaternion,
            "toEulerAngles", &Matrix4::toEulerAngles,

            // Static factory methods
            "translation", sol::overload(
                static_cast<Matrix4(*)(float,float,float)>(&Matrix4::translation),
                static_cast<Matrix4(*)(const Vector3&)>(&Matrix4::translation)
            ),
            "scale", sol::overload(
                static_cast<Matrix4(*)(float,float,float)>(&Matrix4::scale),
                static_cast<Matrix4(*)(const Vector3&)>(&Matrix4::scale)
            ),
            "rotationX", &Matrix4::rotationX,
            "rotationY", &Matrix4::rotationY,
            "rotationZ", &Matrix4::rotationZ,
            "rotationXYZ", sol::overload(
                static_cast<Matrix4(*)(float,float,float)>(&Matrix4::rotationXYZ),
                static_cast<Matrix4(*)(const Vector3)>(&Matrix4::rotationXYZ)
            ),
            "rotationZYX", sol::overload(
                static_cast<Matrix4(*)(float,float,float)>(&Matrix4::rotationZYX),
                static_cast<Matrix4(*)(const Vector3)>(&Matrix4::rotationZYX)
            ),
            "rotationAxis", &Matrix4::rotationAxis,
            "orthographic", sol::overload(
                static_cast<Matrix4(*)(float,float,float,float,float,float)>(&Matrix4::orthographic),
                static_cast<Matrix4(*)(float,float,float,float)>(&Matrix4::orthographic)
            ),
            "perspective", &Matrix4::perspective,
            "lookAt", &Matrix4::lookAt,
            "inverse", &Matrix4::inverse,
            "position", &Matrix4::position
        );
    }

    void Sol2API_Math::setupQuaternion(sol::state& lua) {
        lua.new_usertype<Quaternion>("Quaternion",
            // Constructors
            sol::constructors<
                Quaternion(),
                Quaternion(float, float, float, float),
                Quaternion(VectorTemplated<float,4>)
            >(),

            // Fields via getters/setters
            "w", sol::property(&Quaternion::w, &Quaternion::setW),
            "x", sol::property(&Quaternion::x, &Quaternion::setX),
            "y", sol::property(&Quaternion::y, &Quaternion::setY),
            "z", sol::property(&Quaternion::z, &Quaternion::setZ),

            // Directional vectors
            "forward", [](LaurelEye::Quaternion& q) {
                auto v = q.forward(); // returns VectorTemplated<float, 3>
                return LaurelEye::Vector3(v);
            },
            "right", [](LaurelEye::Quaternion& q) {
                auto v = q.right(); // returns VectorTemplated<float, 3>
                return LaurelEye::Vector3(v);
            },
            "up", [](LaurelEye::Quaternion& q) {
                auto v = q.up(); // returns VectorTemplated<float, 3>
                return LaurelEye::Vector3(v);
            },

            // Operator overloads
            sol::meta_function::multiplication, sol::overload(
                [](const Quaternion& q, const Quaternion& r) {
                    Quaternion copy = q;
                    return copy *= r;
                },
                [](const Quaternion& q, const Vector3& v) {
                    return q * v;
                }
            ),
            sol::meta_function::addition, [](const Quaternion& a, const Quaternion& b) {
                Quaternion copy = a;
                return copy += b;
            },

            // Instance methods
            "inverse", &Quaternion::inverse,
            "conjugate", &Quaternion::conjugate,
            "normalize", &Quaternion::normalize,
            "normalized", &Quaternion::normalized,
            "toEuler", &Quaternion::toEuler,
            "getAxis", &Quaternion::getAxis,

            // Static methods
            "fromEuler", sol::overload(
                static_cast<Quaternion(*)(const Vector3&)>(&Quaternion::fromEuler),
                static_cast<Quaternion(*)(float,float,float)>(&Quaternion::fromEuler)
            ),
            "axisAngleToQuaternion", [](const Vector3& v, float f) -> Quaternion {
                return Quaternion::axisAngleToQuaternion(FVector3(v), f);
            },
            "slerp", &Quaternion::slerp
        );
    }

    void Sol2API_Math::setupTransform(sol::state& lua) {
        lua.new_usertype<Transform>("Transform",
            sol::constructors<
                Transform(),
                Transform(LaurelEye::Vector3, LaurelEye::Quaternion, LaurelEye::Vector3)
            >(),

            // Properties
            "getPosition", &Transform::getPosition,
            "getRotation", &Transform::getRotation,
            "getScaling", &Transform::getScaling,

            "setPosition", sol::overload(
                static_cast<Transform&(Transform::*)(const LaurelEye::Vector3&)>(&Transform::setPosition),
                static_cast<Transform&(Transform::*)(float, float, float)>(&Transform::setPosition)
            ),
            "setRotation", sol::overload(
                static_cast<Transform&(Transform::*)(const LaurelEye::Vector3&)>(&Transform::setRotation),
                static_cast<Transform&(Transform::*)(float, float, float)>(&Transform::setRotation),
                static_cast<Transform&(Transform::*)(const LaurelEye::Quaternion&)>(&Transform::setRotation)
            ),
            "setScaling", sol::overload(
                static_cast<Transform&(Transform::*)(const LaurelEye::Vector3&)>(&Transform::setScaling),
                static_cast<Transform&(Transform::*)(float, float, float)>(&Transform::setScaling)
            ),

            // Operators
            sol::meta_function::multiplication, &Transform::operator*,

            // Local matrices
            "getLocalMatrix", &Transform::getLocalMatrix,
            "getInverseLocalMatrix", &Transform::getInverseLocalMatrix,

            // Translate / Rotate / Scale methods
            "translate", sol::overload(
                static_cast<Transform&(Transform::*)(const LaurelEye::Vector3&)>(&Transform::translate),
                static_cast<Transform&(Transform::*)(float,float,float)>(&Transform::translate)
            ),
            "rotate", sol::overload(
                static_cast<Transform&(Transform::*)(const LaurelEye::Quaternion&)>(&Transform::rotate),
                static_cast<Transform&(Transform::*)(const LaurelEye::Vector3&)>(&Transform::rotate),
                static_cast<Transform&(Transform::*)(float,float,float)>(&Transform::rotate)
            ),
            "scale", sol::overload(
                static_cast<Transform&(Transform::*)(const LaurelEye::Vector3&)>(&Transform::scale),
                static_cast<Transform&(Transform::*)(float,float,float)>(&Transform::scale)
            )
        );
    }
} // namespace LaurelEye::Scripting
