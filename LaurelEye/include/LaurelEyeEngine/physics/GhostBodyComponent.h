///
/// @file   GhostBodyComponent.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   02-04-2026
/// @brief  
/// 
/// 
///

#pragma once

#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
#include "LaurelEyeEngine/physics/interfaces/IGhostBody.h"

namespace LaurelEye::Physics {

    //class PhysicsSystem;
    class GhostBodyComponent : public PhysicsBodyBaseComponent {

    public:
        explicit GhostBodyComponent(const PhysicsBodyData& d);

        std::shared_ptr<ICollider> GetCollider() const override;

        //--Overrides
        void CreateInWorld(PhysicsSystem& system) override;
        void RemoveFromWorld(PhysicsSystem& system) override;
        
        void SyncToPhysics() override;
        void SyncFromPhysics() override;

        void setOwner(Entity* entity) override;

        void SetLinearVeloctiy(Vector3 newVelocity) override;
        Vector3 GetLinearVelocity() override;

        void SetAngularVelocity(Vector3 newVelocity) override;
        Vector3 GetAngularVelocity() override;

        void ApplyForce(Vector3 f) override;
        void ApplyImpulse(Vector3 i) override;

        Vector3 getGravityScale() override;
        void setGravityScale(Vector3 nv) override;

    private:
        std::shared_ptr<IGhostBody> ghost;

    };

}
