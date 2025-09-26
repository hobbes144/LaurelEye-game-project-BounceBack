#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletBody.h"

namespace LaurelEye::Physics {

    static btVector3 ToBt(const Vector3& v) {
        return {v.x, v.y, v.z};
    }
    static Vector3 FromBt(const btVector3& v) {
        return {v.x(), v.y(), v.z()};
    }
    static btQuaternion ToBt(const Quaternion& q) {
        return {q.x(), q.y(), q.z(), q.w()};
    }
    static Quaternion FromBt(const btQuaternion& q) {
        return {q.w(), q.x(), q.y(), q.z()};
    }

    BulletBody::BulletBody(std::shared_ptr<btRigidBody> body, std::shared_ptr<BulletShape> s, std::shared_ptr<btDefaultMotionState> motion)
        : btRB(std::move(body)), shape(std::move(s)), motionState(std::move(motion)){}

    BulletBody::~BulletBody() {

    }

    void BulletBody::ApplyForce(const Vector3& f) {
        btRB->applyCentralForce(ToBt(f));
    }

    void BulletBody::ApplyImpulse(const Vector3& i) {
        btRB->applyCentralImpulse(ToBt(i));
    }

    void BulletBody::SetTransform(const TransPhys& t) {
        btTransform trans(ToBt(t.rotation), ToBt(t.position));
    }

    TransPhys BulletBody::GetTransform() const {
        btTransform trans;
        btRB->getMotionState()->getWorldTransform(trans);
        return { FromBt(trans.getOrigin()), FromBt(trans.getRotation()) };
    }

    void BulletBody::SetVelocity(const Vector3& v) {
        btRB->setLinearVelocity(ToBt(v));
    }

    Vector3 BulletBody::GetVelocity() const {
        return FromBt(btRB->getLinearVelocity());
    }

    void BulletBody::SetMass(float m) {
        btVector3 inertia(0, 0, 0);
        btRB->getCollisionShape()->calculateLocalInertia(m, inertia);
        btRB->setMassProps(m, inertia);
    }

    float BulletBody::GetMass() const {
        return 1.0f / btRB->getInvMass();
    }

}
