///
/// @file   PhysicsBodyBaseComponent.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   02-06-2026
/// @brief
///
///
///

#pragma once

#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"
//#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/ecs/IComponent.h"
#include <memory>

namespace LaurelEye::Physics {

    class PhysicsSystem;
    class ICollider;
    class PhysicsBodyBaseComponent : public IComponent {
    public:
        virtual ~PhysicsBodyBaseComponent() = default;

        PhysicsBodyData& GetBodyData() { return data; }
        void SetBodyData(const PhysicsBodyData& d) {
            data = d;
        }

        virtual std::shared_ptr<ICollider> GetCollider() const = 0;

        virtual void CreateInWorld(PhysicsSystem& system) = 0;
        virtual void RemoveFromWorld(PhysicsSystem& system) = 0;

        virtual void SyncToPhysics() = 0;
        virtual void SyncFromPhysics() = 0;

        virtual void SetLinearVeloctiy(Vector3 newVelocity) = 0;
        virtual Vector3 GetLinearVelocity() = 0;

        virtual void SetAngularVelocity(Vector3 newVelocity) = 0;
        virtual Vector3 GetAngularVelocity() = 0;

        virtual void ApplyForce(Vector3 f) = 0;
        virtual void ApplyImpulse(Vector3 i) = 0;


    protected:
        PhysicsBodyData data;

    };

}
