#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletWorld.h"

namespace LaurelEye::Physics {
    PhysicsSystem::PhysicsSystem(PhysicsSystemType type) {
        systemType = type;
    }

    void PhysicsSystem::initialize() {
        switch ( systemType ) {
        case PhysicsSystemType::Bullet:
            world = std::make_unique<BulletWorld>();
            break;
            // Add more backends here
        }
    }

    void PhysicsSystem::update(float dt) {
        if ( world ) world->StepSimulation(dt);
    }

    void PhysicsSystem::shutdown() {

    }

    std::shared_ptr<IBody> PhysicsSystem::CreateBody(
        BodyType type,
        const CollisionShapePhys& shapeDesc,
        const TransPhys& transform) {
        return world ? world->CreateBody(type, shapeDesc, transform) : nullptr;
    }

    std::shared_ptr<ICollisionShape> PhysicsSystem::CreateShape(const CollisionShapePhys& desc) {
        return world ? world->CreateShape(desc) : nullptr;
    }

    IPhysicsWorld* PhysicsSystem::GetWorld() {
        return world.get();
    }
}
