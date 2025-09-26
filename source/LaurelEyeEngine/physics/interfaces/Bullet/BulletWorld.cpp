#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletWorld.h"

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


    void BulletWorld::StepSimulation(float dt) {
        world->stepSimulation(dt);
    }

    std::shared_ptr<IBody> BulletWorld::CreateBody(
        BodyType type,
        const CollisionShapePhys& shapeDesc,
        const TransPhys& start) {

        auto bulletShape = std::dynamic_pointer_cast<BulletShape>(CreateShape(shapeDesc));
        if ( !bulletShape ) throw std::runtime_error("Failed to create BulletShape");

        btTransform btStart;
        btStart.setOrigin(btVector3(start.position.x, start.position.y, start.position.z));
        btStart.setRotation(btQuaternion(start.rotation.x(), start.rotation.y(), start.rotation.z(), start.rotation.w()));

        float mass = (type == BodyType::Dynamic) ? 1.0f : 0.0f;
        btVector3 inertia(0, 0, 0);
        if ( mass > 0.0f ) bulletShape->GetInternal()->calculateLocalInertia(mass, inertia);

        auto motion = std::make_shared<btDefaultMotionState>(btStart);
        motions.push_back(motion); // keep alive
        btRigidBody::btRigidBodyConstructionInfo rbInfo(
            mass,
            motion.get(),
            bulletShape->GetInternal(),
            inertia);

        auto body = std::make_shared<btRigidBody>(rbInfo);
        world->addRigidBody(body.get());

        auto newBody = std::make_shared<BulletBody>(body, bulletShape, motion);
        bodies.push_back(newBody);
        return newBody;
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


}
