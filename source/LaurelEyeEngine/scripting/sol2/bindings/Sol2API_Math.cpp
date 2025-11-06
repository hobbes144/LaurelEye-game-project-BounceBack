#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math.h"

namespace LaurelEye::Scripting {

    void Sol2API_Math::setup(sol::state& lua) {
        // Vector3
        setupVec3(lua);

        // Matrices
        setupMatrix4(lua);

        // Quaternion
        setupQuaternion(lua);

        // Transform
        setupTransform(lua);
    }

    void Sol2API_Math::setupVec3(sol::state& lua) {
        lua.new_usertype<LaurelEye::Vector3>("Vector3",
        // Constructors
        sol::constructors<
            LaurelEye::Vector3(),
            LaurelEye::Vector3(float, float, float),
            LaurelEye::Vector3(float)>(),

        // Fields
        "x", &LaurelEye::Vector3::x,
        "y", &LaurelEye::Vector3::y,
        "z", &LaurelEye::Vector3::z,

        // Methods
        "Dot", &LaurelEye::Vector3::dot,
        "Magnitude", &LaurelEye::Vector3::magnitude,
        "MagnitudeSquared", &LaurelEye::Vector3::magnitudSquared,
        "Normalized", &LaurelEye::Vector3::normalized,
        "Cross", &LaurelEye::Vector3::cross,
        "Reciprocal", &LaurelEye::Vector3::reciprocal,
        "Project", &LaurelEye::Vector3::project,
        "Abs", &LaurelEye::Vector3::abs,

        // Operator overloads
        sol::meta_function::addition, sol::overload([](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a + b; }, [](const LaurelEye::Vector3& a, float b) { return a + b; }),
        sol::meta_function::subtraction, sol::overload([](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a - b; }, [](const LaurelEye::Vector3& a, float b) { return a - b; }),
        sol::meta_function::multiplication, sol::overload([](const LaurelEye::Vector3& a, float b) { return a * b; }, [](float b, const LaurelEye::Vector3& a) { return a * b; }, [](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a * b; }),
        sol::meta_function::division, sol::overload([](const LaurelEye::Vector3& a, float b) { return a / b; }, [](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a / b; }),
        sol::meta_function::equal_to, &LaurelEye::Vector3::operator==);

        auto v3Table = lua["Vector3"];
        v3Table["Zero"] = LaurelEye::Vector3(0, 0, 0);
        v3Table["One"] = LaurelEye::Vector3(1, 1, 1);
        v3Table["Up"] = LaurelEye::Vector3(0, 1, 0);
        v3Table["Down"] = LaurelEye::Vector3(0, -1, 0);
        v3Table["Right"] = LaurelEye::Vector3(1, 0, 0);
        v3Table["Left"] = LaurelEye::Vector3(-1, 0, 0);
        v3Table["Forward"] = LaurelEye::Vector3(0, 0, 1);
        v3Table["Back"] = LaurelEye::Vector3(0, 0, -1);
    }

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
            "orthographic", &Matrix4::orthographic,
            "perspective", &Matrix4::perspective,
            "lookAt", &Matrix4::lookAt,
            "inverse", &Matrix4::inverse
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
            "forward", &Quaternion::forward,
            "right", &Quaternion::right,
            "up", &Quaternion::up,

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
            "axisAngleToQuaternion", &Quaternion::axisAngleToQuaternion
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

