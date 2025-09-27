#pragma once

#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"

namespace LaurelEye::Physics {

    class IBody {
    public:
        virtual ~IBody() = default;

        virtual void SetTransform(const TransPhys& t) = 0;
        virtual TransPhys GetTransform() const = 0;

        virtual void ApplyForce(const Vector3& f) = 0;
        virtual void ApplyImpulse(const Vector3& f) = 0;

        virtual void SetVelocity(const Vector3& v) = 0;
        virtual Vector3 GetVelocity() const = 0;

        virtual void SetMass(float m) = 0;
        virtual float GetMass() const = 0;
    };

} // namespace LaurelEye::Physics
