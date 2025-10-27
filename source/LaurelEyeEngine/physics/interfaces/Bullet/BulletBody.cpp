#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletBody.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"

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

    BulletBody::~BulletBody() {}

    void BulletBody::ApplyForce(const Vector3& f) {
        btRB->activate(true);
        btRB->applyCentralForce(ToBt(f));
    }

    void BulletBody::ApplyImpulse(const Vector3& i) {
        btRB->activate(true);
        btRB->applyCentralImpulse(ToBt(i));
    }

    void BulletBody::BindTransform(LaurelEye::TransformComponent* t) {
        boundTransform = t;
    }

    LaurelEye::TransformComponent* BulletBody::GetBoundTransform() const {
        return boundTransform;
    }

    void BulletBody::BindPhysicsBodyComponent(PhysicsBodyComponent* pbc) {
        btRB->setUserPointer(pbc);
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

    void BulletBody::pushTransformToPhysics() {
        if ( !btRB || !boundTransform ) return;
        const auto& pos = boundTransform->getWorldTransform().getPosition();
        const auto& rot = boundTransform->getWorldTransform().getRotation();
        btTransform btTrans(btQuaternion(rot.x(), rot.y(), rot.z(), rot.w()),
                            btVector3(pos.x, pos.y, pos.z));
        btRB->setWorldTransform(btTrans);
        btRB->getMotionState()->setWorldTransform(btTrans);
    }

    void BulletBody::updateTransformFromPhysics() {
        if ( !btRB || !boundTransform ) return;
        btTransform btTrans = btRB->getWorldTransform();
        btVector3 pos = btTrans.getOrigin();
        btQuaternion rot = btTrans.getRotation();
        float temp = pos.y();
        boundTransform->setLocalPosition(Vector3(pos.x(), pos.y(), pos.z()));
        boundTransform->setLocalRotation(Quaternion(rot.x(), rot.y(), rot.z(), rot.w()));
    }

}
