#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletWorld.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletBody.h"
#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"

namespace LaurelEye::Physics {

    BulletWorld::BulletWorld() {
        config = std::make_unique<btDefaultCollisionConfiguration>();
        dispatcher = std::make_unique<btCollisionDispatcher>(config.get());
        broadphase = std::make_unique<btDbvtBroadphase>();
        solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        world = std::make_unique<btDiscreteDynamicsWorld>(
            dispatcher.get(), broadphase.get(), solver.get(), config.get());
        world->setGravity(btVector3(0, -9.81f, 0));
    }

    BulletWorld::~BulletWorld() {
        for ( auto& body : bodies ) {
            auto b = std::dynamic_pointer_cast<BulletBody>(body);
            if ( b && b->GetInternal() ) world->removeRigidBody(b->GetInternal().get());
        }

        // Clear smart pointer containers (shared_ptrs will clean up)
        bodies.clear();
        motions.clear();
        shapes.clear();

        // Delete Bullet world components in reverse order
        world.reset();
        solver.reset();
        broadphase.reset();
        dispatcher.reset();
        config.reset();
    }


    void BulletWorld::StepSimulation(float dt, int maxSubSteps, float fixedTimeStep) {
        //Run Simlation
        world->stepSimulation(dt, maxSubSteps, fixedTimeStep);
    }

    std::shared_ptr<IBody> BulletWorld::CreateBody(const PhysicsBodyData& data) {

        //Shape
        auto bulletShape = std::dynamic_pointer_cast<BulletShape>(CreateShape(data.shapeDefinition));
        if ( !bulletShape ) throw std::runtime_error("Failed to create BulletShape");

        // Apply local scaling from transform
        if ( data.transformRef ) {
            Vector3 scale = data.transformRef->getWorldScale();
            btVector3 dim = bulletShape->GetInternal()->getLocalScaling();
            bulletShape->GetInternal()->setLocalScaling(btVector3(scale.x * dim.x(), scale.y * dim.y(), scale.z)*dim.z());
        }

        //Start Transform
        btTransform btStart;
        btStart.setOrigin(btVector3(
            data.transformRef->getWorldPosition().x,
            data.transformRef->getWorldPosition().y,
            data.transformRef->getWorldPosition().z));
        btStart.setRotation(btQuaternion(
            data.transformRef->getWorldRotation().x(),
            data.transformRef->getWorldRotation().y(),
            data.transformRef->getWorldRotation().z(),
            data.transformRef->getWorldRotation().w()));

        //Mass & Inertia
        float mass = (data.type == BodyType::Dynamic) ? data.mass : 0.0f;

        //Center of Mass
        btCollisionShape* shape = bulletShape->GetInternal();
        if ( !(data.centerOfMass == Vector3(0, 0, 0)) ) {
            btCompoundShape* compound = new btCompoundShape();

            btTransform offset;
            offset.setIdentity();
            offset.setOrigin(btVector3(
                data.centerOfMass.x,
                data.centerOfMass.y,
                data.centerOfMass.z));

            compound->addChildShape(offset, shape);

            // Replace internal shape
            bulletShape->SetInternal(compound);
            shape = compound;
        }


        btVector3 linearVelocity(data.linearVelocity.x, data.linearVelocity.y, data.linearVelocity.z);
        btVector3 angularVelocity(data.angularVelocity.x, data.angularVelocity.y, data.angularVelocity.z);

        btVector3 inertia(0, 0, 0);
        if ( mass > 0.0f ) bulletShape->GetInternal()->calculateLocalInertia(mass, inertia);
        if ( !(data.inertia == Vector3(0, 0, 0)) ) {
            inertia = btVector3(data.inertia.x, data.inertia.y, data.inertia.z);
        }

        //Motion State
        auto motion = std::make_shared<btDefaultMotionState>(btStart);
        motions.push_back(motion); // keep alive

        //Body Info
        btRigidBody::btRigidBodyConstructionInfo rbInfo(
            mass,
            motion.get(),
            bulletShape->GetInternal(),
            inertia);
        rbInfo.m_linearDamping = data.linearDamping;
        rbInfo.m_angularDamping = data.angularDamping;
        rbInfo.m_friction = data.friction;
        rbInfo.m_restitution = data.restitution;

        //Create and Add Rigidbody
        auto body = std::make_shared<btRigidBody>(rbInfo);
        body->setLinearVelocity(btVector3(data.linearVelocity.x, data.linearVelocity.y, data.linearVelocity.z));
        body->setAngularVelocity(btVector3(data.angularVelocity.x, data.angularVelocity.y, data.angularVelocity.z));
        body->setFriction(data.friction);
        body->setRestitution(data.restitution);
        world->addRigidBody(body.get(), data.collisionGroup, data.collisionMask);

        //Continuous Collision Detection
        if ( data.useCCD ) {
            body->setCcdMotionThreshold(data.ccdMotionThreshold);
            body->setCcdSweptSphereRadius(data.ccdSweptSphereRadius);
        }

        //Gravity Scale
        if ( data.gravityScale != 1.0f ) {
            btVector3 worldGravity = world->getGravity();
            body->setGravity(worldGravity * data.gravityScale);
        }

        //Save
        auto newBody = std::make_shared<BulletBody>(body, bulletShape, motion);
        bodies.push_back(newBody);

        return newBody;
    }

    void BulletWorld::RemoveBody(std::shared_ptr<IBody> body) {
        if ( !body ) return;
        auto bulletBody = std::dynamic_pointer_cast<BulletBody>(body);
        if ( !bulletBody ) return;

        auto internal = bulletBody->GetInternal();
        if ( internal ) world->removeRigidBody(internal.get());

        // Erase body
        bodies.erase(
            std::remove_if(bodies.begin(), bodies.end(),
                           [&](const std::shared_ptr<IBody>& b) { return b == body; }),
            bodies.end());
    }

    std::shared_ptr<ICollisionShape> BulletWorld::CreateShape(const CollisionShapePhys& csp) {
        btCollisionShape* btShape = nullptr;
        switch ( csp.type ) {
        case CollisionShapePhys::ShapeType::Box:
            btShape = new btBoxShape(btVector3(csp.size.x / 2, csp.size.y / 2, csp.size.z / 2));
            break;
        case CollisionShapePhys::ShapeType::Sphere:
            btShape = new btSphereShape(csp.radius);
            break;
        case CollisionShapePhys::ShapeType::Capsule:
            btShape = new btCapsuleShape(csp.radius, csp.height);
            break;
        default:
            btShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
            break;
        }
        auto shape = std::make_shared<BulletShape>(csp, btShape);
        shapes.push_back(shape); // keep it alive
        return shape;
    }

    void BulletWorld::GatherCollisions(CollisionManager& cm) {
        int numManifolds = dispatcher->getNumManifolds();
        for ( int i = 0; i < numManifolds; i++ ) {
            btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);
            const btCollisionObject* obj0 = manifold->getBody0();
            const btCollisionObject* obj1 = manifold->getBody1();

            auto body0 = static_cast<PhysicsBodyComponent*>(obj0->getUserPointer());
            auto body1 = static_cast<PhysicsBodyComponent*>(obj1->getUserPointer());
            if ( !body0 || !body1 ) continue;

            int numContacts = manifold->getNumContacts();
            for ( int j = 0; j < numContacts; ++j ) {
                btManifoldPoint& pt = manifold->getContactPoint(0);
                Vector3 contactPoint(pt.getPositionWorldOnB().x(),
                                     pt.getPositionWorldOnB().y(),
                                     pt.getPositionWorldOnB().z());
                Vector3 normal(pt.m_normalWorldOnB.x(),
                               pt.m_normalWorldOnB.y(),
                               pt.m_normalWorldOnB.z());

                cm.registerContact(
                    body0->GetBodyRef()->GetBodyID(),
                    body1->GetBodyRef()->GetBodyID(),
                    contactPoint,
                    normal,
                    body0->getOwner(),
                    body1->getOwner()
                );
            }
        }
    }

    RaycastHit BulletWorld::Raycast(const Vector3& origin,
                                    const Vector3& direction,
                                    float maxDistance,
                                    const RaycastParams& params) const {
        RaycastHit out{};

        Vector3 dirN = direction.normalized();
        Vector3 end = origin + dirN * maxDistance;

        btVector3 from(origin.x, origin.y, origin.z);
        btVector3 to(end.x, end.y, end.z);

        btCollisionWorld::ClosestRayResultCallback cb(from, to);

        // Bullet filtering: mask/group.
        cb.m_collisionFilterMask = params.layerMask;
        // cb.m_collisionFilterGroup = ??? (rarely needed for raycasts)

        world->rayTest(from, to, cb);

        if ( !cb.hasHit() )
            return out;

        out.hit = true;
        out.position = Vector3(cb.m_hitPointWorld.x(), cb.m_hitPointWorld.y(), cb.m_hitPointWorld.z());
        out.normal = Vector3(cb.m_hitNormalWorld.x(), cb.m_hitNormalWorld.y(), cb.m_hitNormalWorld.z());
        out.distance = maxDistance * cb.m_closestHitFraction;

        const btCollisionObject* obj = cb.m_collisionObject;
        auto body = static_cast<PhysicsBodyComponent*>(obj->getUserPointer());
        out.entityRef = body->getOwner();

        return out;
    }
}
