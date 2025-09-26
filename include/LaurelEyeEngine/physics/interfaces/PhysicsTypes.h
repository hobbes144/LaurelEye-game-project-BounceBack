#pragma once

#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Quaternion.h"

namespace LaurelEye::Physics {

    using PhysicsId = std::uint32_t;

    enum class BodyType {
        Static, Dynamic, Kinematic
    };

    struct TransPhys {
        LaurelEye::Vector3 position;
        LaurelEye::Quaternion rotation;
    };

    struct CollisionShapePhys {
        enum class ShapeType {Box, Sphere, Capsule, Mesh } type;
        Vector3 size = Vector3(1, 1, 1);
        float radius = 0.5f;
        float height = 1.0f;
    };

}
