#pragma once

#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"

namespace LaurelEye::Physics {

    class ICollisionShape {
    public:
        virtual ~ICollisionShape() = default;
        virtual CollisionShapePhys GetDiscription() const = 0;
    };

}
