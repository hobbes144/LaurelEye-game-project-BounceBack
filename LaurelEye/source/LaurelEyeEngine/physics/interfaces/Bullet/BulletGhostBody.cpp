#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletGhostBody.h"
#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
//#include "LaurelEyeEngine/physics/interfaces/ICollider.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

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

    BulletGhostBody::BulletGhostBody(std::shared_ptr<btRigidBody> g,
                                     std::shared_ptr<BulletShape> s)
        : ghost(g), shape(s) {

        ghost->setCollisionFlags(
            ghost->getCollisionFlags() |
            btCollisionObject::CF_NO_CONTACT_RESPONSE
        );

    }

    void BulletGhostBody::BindTransform(TransformComponent* t) {
        boundTransform = t;
    }

    TransformComponent* BulletGhostBody::GetBoundTransform() const {
        return boundTransform;
    }

    void BulletGhostBody::pushTransformToPhysics() {
        if ( !ghost || !boundTransform ) return;
        const auto& pos = boundTransform->getWorldTransform().getPosition();
        const auto& rot = boundTransform->getWorldTransform().getRotation();
        btTransform btTrans(btQuaternion(rot.x(), rot.y(), rot.z(), rot.w()),
                            btVector3(pos.x, pos.y, pos.z));
        ghost->setWorldTransform(btTrans);
        ghost->getMotionState()->setWorldTransform(btTrans);
    }

    void BulletGhostBody::updateTransformFromPhysics() {
        if ( !ghost || !boundTransform ) return;
        btTransform btTrans = ghost->getWorldTransform();
        btVector3 pos = btTrans.getOrigin();
        btQuaternion rot = btTrans.getRotation();
        float temp = pos.y();
        // boundTransform->setWorldPosition(Vector3(pos.x(), pos.y(), pos.z()));
        // boundTransform->setWorldRotation(Quaternion(rot.w(), rot.x(), rot.y(), rot.z()));
        boundTransform->setAbsoluteWorldPosRot(Vector3(pos.x(), pos.y(), pos.z()), Quaternion(rot.w(), rot.x(), rot.y(), rot.z()));
    }

    std::vector<ICollider*> BulletGhostBody::GetOverlaps() const {
        /*
        cached.clear();

        btDispatcher* dispatcher =
            ghost->getWorldArrayIndex() >= 0
                ? ghost->getBroadphaseHandle()->m_clientObject->getWorld()->getDispatcher()
                : nullptr;

        if ( !dispatcher ) return cached;

        int numManifolds = dispatcher->getNumManifolds();
        for ( int i = 0; i < numManifolds; ++i ) {
            btPersistentManifold* m = dispatcher->getManifoldByIndexInternal(i);

            btCollisionObject* a = m->getBody0();
            btCollisionObject* b = m->getBody1();

            if ( a != ghost.get() && b != ghost.get() ) continue;

            btCollisionObject* other = (a == ghost.get()) ? b : a;

            auto* comp = static_cast<PhysicsBodyBaseComponent*>(
                other->getUserPointer());

            if ( !comp ) continue;

            auto collider = comp->GetCollider();
            if ( collider ) cached.push_back(collider.get());
        }

        */

        LE_DEBUG_ERROR("Physics", "GetOverlaps is not Implemented");

        return cached;
    }


    int BulletGhostBody::GetNumberOverlappingObjects() const {
        return GetOverlaps().size();
    }

    void* BulletGhostBody::GetUserData() const {
        return ghost->getUserPointer();
    }

    void BulletGhostBody::SetUserData(void* ptr) {
        ghost->setUserPointer(ptr);
    }

    void BulletGhostBody::SetLinearVelocity(const Vector3& v) {
        ghost->activate(true);
        ghost->setLinearVelocity(ToBt(v));
    }

    Vector3 BulletGhostBody::GetLinearVelocity() const {
        return (FromBt(ghost->getLinearVelocity()));
    }

    void BulletGhostBody::SetAngularVelocity(const Vector3& v) {
        ghost->activate(true);
        ghost->setLinearVelocity(ToBt(v));
    }

    Vector3 BulletGhostBody::GetAngularVelocity() const {
        return (FromBt(ghost->getAngularVelocity()));
    }

    std::shared_ptr<btRigidBody> BulletGhostBody::GetInternal() {
        return ghost;
    }

    Vector3 BulletGhostBody::GetGravityScale() const {
        return FromBt(ghost->getGravity());
    }

    void BulletGhostBody::SetGravityScale(Vector3 nv) {
        ghost->setGravity(ToBt(nv));
    }

}
