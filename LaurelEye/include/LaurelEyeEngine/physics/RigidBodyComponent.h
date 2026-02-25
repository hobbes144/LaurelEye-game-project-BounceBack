/// @file   RigidBodyComponent.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief ECS component that stores physics body data
/// and a reference to the backend body object.
/// Acts as the link between entities and the physics system.

#pragma once

#include <memory>

#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
#include "LaurelEyeEngine/physics/interfaces/IRigidBody.h"
#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"

namespace LaurelEye::Physics {

    class RigidBodyComponent : public PhysicsBodyBaseComponent {
    public:
        RigidBodyComponent();
        explicit RigidBodyComponent(const PhysicsBodyData& initData);

        //--Override Functionality
        std::shared_ptr<ICollider> GetCollider() const override;

        void CreateInWorld(PhysicsSystem& system) override;
        void RemoveFromWorld(PhysicsSystem& system) override;

        void SyncToPhysics() override;
        void SyncFromPhysics() override;

        //--Rigid Body Specific
        //std::shared_ptr<IBody> GetBodyRef();
        //void SetBodyRef(std::shared_ptr<IBody> b);

        void setOwner(Entity* entity) override;

        void SetLinearVeloctiy(Vector3 newVelocity) override;
        Vector3 GetLinearVelocity() override;

        void SetAngularVelocity(Vector3 newVelocity) override;
        Vector3 GetAngularVelocity() override;

        void ApplyForce(Vector3 f) override;
        void ApplyImpulse(Vector3 i) override;

    private:
        std::shared_ptr<IRigidBody> rigidBody;

    };

}
