#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletRigidBody.h"
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

    BulletRigidBody::BulletRigidBody(std::shared_ptr<btRigidBody> body, std::shared_ptr<BulletShape> s, std::shared_ptr<btDefaultMotionState> motion)
        : btRB(std::move(body)), shape(std::move(s)), motionState(std::move(motion)){}

    BulletRigidBody::~BulletRigidBody() {}

    void BulletRigidBody::ApplyForce(const Vector3& f) {
        btRB->activate(true);
        btRB->applyCentralForce(ToBt(f));
    }

    void BulletRigidBody::ApplyImpulse(const Vector3& i) {
        btRB->activate(true);
        btRB->applyCentralImpulse(ToBt(i));
    }

    void BulletRigidBody::BindTransform(LaurelEye::TransformComponent* t) {
        boundTransform = t;
    }

    LaurelEye::TransformComponent* BulletRigidBody::GetBoundTransform() const {
        return boundTransform;
    }

    /*
    void BulletRigidBody::BindPhysicsBodyComponent(PhysicsBodyBaseComponent* pbc) {
        btRB->setUserPointer(pbc);
    }
    */

    void BulletRigidBody::SetLinearVelocity(const Vector3& v) {
        btRB->activate(true);
        btRB->setLinearVelocity(ToBt(v));
    }

    Vector3 BulletRigidBody::GetLinearVelocity() const {
        return FromBt(btRB->getLinearVelocity());
    }

    void BulletRigidBody::SetAngularVelocity(const Vector3& v) {
        btRB->activate(true);
        btRB->setAngularVelocity(ToBt(v));
    }

    Vector3 BulletRigidBody::GetAngularVelocity() const {
        return FromBt(btRB->getAngularVelocity());
    }

    void BulletRigidBody::SetMass(float m) {
        btVector3 inertia(0, 0, 0);
        btRB->getCollisionShape()->calculateLocalInertia(m, inertia);
        btRB->setMassProps(m, inertia);
    }

    float BulletRigidBody::GetMass() const {
        return 1.0f / btRB->getInvMass();
    }

    Vector3 BulletRigidBody::GetGravityScale() const {
        return FromBt(btRB->getGravity());
    }

    void BulletRigidBody::SetGravityScale(Vector3 nv) {
        btRB->setGravity(ToBt(nv));
    }

    void BulletRigidBody::pushTransformToPhysics() {
        if ( !btRB || !boundTransform ) return;
        const auto& pos = boundTransform->getWorldTransform().getPosition();
        const auto& rot = boundTransform->getWorldTransform().getRotation();
        btTransform btTrans(btQuaternion(rot.x(), rot.y(), rot.z(), rot.w()),
                            btVector3(pos.x, pos.y, pos.z));
        btRB->setWorldTransform(btTrans);
        btRB->getMotionState()->setWorldTransform(btTrans);
    }

    void BulletRigidBody::updateTransformFromPhysics() {
        if ( !btRB || !boundTransform ) return;
        btTransform btTrans = btRB->getWorldTransform();
        btVector3 pos = btTrans.getOrigin();
        btQuaternion rot = btTrans.getRotation();
        float temp = pos.y();
        //boundTransform->setWorldPosition(Vector3(pos.x(), pos.y(), pos.z()));
        //boundTransform->setWorldRotation(Quaternion(rot.w(), rot.x(), rot.y(), rot.z()));
        boundTransform->setAbsoluteWorldPosRot(Vector3(pos.x(), pos.y(), pos.z()), Quaternion(rot.w(), rot.x(), rot.y(), rot.z()));
    }

    void* BulletRigidBody::GetUserData() const {
        return btRB->getUserPointer();
    }

    void BulletRigidBody::SetUserData(void* ptr) {
        btRB->setUserPointer(ptr);
    }


}
