#pragma once

#include "LaurelEyeEngine/physics/interfaces/IRigidBody.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletShape.h"

#include <btBulletDynamicsCommon.h>
#include <memory>

namespace LaurelEye::Physics {
    class BulletRigidBody final : public IRigidBody {
    public:
        BulletRigidBody(std::shared_ptr<btRigidBody> body, std::shared_ptr<BulletShape> s, std::shared_ptr<btDefaultMotionState> motion);
        ~BulletRigidBody() override;

        void ApplyForce(const Vector3& f) override;
        void ApplyImpulse(const Vector3& i) override;

        void BindTransform(LaurelEye::TransformComponent* t) override;
        LaurelEye::TransformComponent* GetBoundTransform() const override;

        void pushTransformToPhysics() override;
        void updateTransformFromPhysics() override;

        void* GetUserData() const override;
        void SetUserData(void*) override;

        //void BindPhysicsBodyComponent(RigidBodyComponent* pbc) override;

        void SetLinearVelocity(const Vector3& v) override;
        Vector3 GetLinearVelocity() const override;

        void SetAngularVelocity(const Vector3& v) override;
        Vector3 GetAngularVelocity() const override;

        void SetMass(float m) override;
        float GetMass() const override;

        Vector3 GetGravityScale() const override;
        void SetGravityScale(Vector3 nv) override;

        std::shared_ptr<btRigidBody> GetInternal() { return btRB; }
        std::shared_ptr<BulletShape> GetShape() { return shape; }

    private:
        std::shared_ptr<btRigidBody> btRB;
        TransformComponent* boundTransform = nullptr;
        std::shared_ptr<BulletShape> shape;
        std::shared_ptr<btDefaultMotionState> motionState;
    };
}
