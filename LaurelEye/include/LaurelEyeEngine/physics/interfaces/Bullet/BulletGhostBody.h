///
/// @file   BulletGhostBody.h
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

#include "LaurelEyeEngine/physics/interfaces/IGhostBody.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletShape.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace LaurelEye::Physics {

    class ICollider;
    class BulletGhostBody final : public IGhostBody {

    public:
        BulletGhostBody(std::shared_ptr<btRigidBody> g,
                        std::shared_ptr<BulletShape> s);

        ~BulletGhostBody() override = default;

        //--Overrides--
        void BindTransform(TransformComponent* t) override;
        TransformComponent* GetBoundTransform() const override;

        void pushTransformToPhysics() override;
        void updateTransformFromPhysics() override;
        //ICollider
        void* GetUserData() const override;
        void SetUserData(void* ptr) override;

        /// @brief Set the linear velocity of this body.
        /// @param v The velocity vector.
        void SetLinearVelocity(const Vector3& v) override;
        /// @brief Get the current linear velocity of this body.
        /// @return The velocity vector.
        Vector3 GetLinearVelocity() const override;

        /// @brief Set the linear velocity of this body.
        /// @param v The velocity vector.
        void SetAngularVelocity(const Vector3& v) override;
        /// @brief Get the current linear velocity of this body.
        /// @return The velocity vector.
        Vector3 GetAngularVelocity() const override;

        std::vector<ICollider*> GetOverlaps() const override;
        int GetNumberOverlappingObjects() const override;

        std::shared_ptr<btRigidBody> GetInternal();
        
    private:
        std::shared_ptr<btRigidBody> ghost;
        std::shared_ptr<BulletShape> shape;
        TransformComponent* boundTransform = nullptr;
        mutable std::vector<ICollider*> cached;
        Vector3 velocity = {0, 0, 0};

    };

}
