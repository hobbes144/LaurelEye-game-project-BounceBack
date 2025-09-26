#pragma once

#include <btBulletDynamicsCommon.h>

#include "LaurelEyeEngine/physics/interfaces/IBody.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletShape.h"

namespace LaurelEye::Physics {
    class BulletBody : public IBody {
    public:
        BulletBody(std::shared_ptr<btRigidBody> body, std::shared_ptr<BulletShape> s, std::shared_ptr<btDefaultMotionState> motion);
        ~BulletBody() override;

        void ApplyForce(const Vector3& f) override;
        void ApplyImpulse(const Vector3& i) override;

        void SetTransform(const TransPhys& t) override;
        TransPhys GetTransform() const override;

        void SetVelocity(const Vector3& v) override;
        Vector3 GetVelocity() const override;

        void SetMass(float m) override;
        float GetMass() const override;

        std::shared_ptr<btRigidBody> GetInternal() { return btRB; }
        std::shared_ptr<BulletShape> GetShape() { return shape; }

    private:
        std::shared_ptr<btRigidBody> btRB;
        std::shared_ptr<BulletShape> shape;
        std::shared_ptr<btDefaultMotionState> motionState;
    };
}
