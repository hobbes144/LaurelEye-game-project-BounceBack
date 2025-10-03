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
        // Sync Transforms to Physics Simulation
        for ( auto* comp : components ) {
            comp->GetBodyRef()->pushTransformToPhysics();
        }

        if ( world ) world->StepSimulation(dt);

        //Sync Physics to Transforms
        for ( auto* comp : components ) {
            comp->GetBodyRef()->updateTransformFromPhysics();
        }

    }

    void PhysicsSystem::shutdown() {

    }

    void PhysicsSystem::registerComponent(const ComponentPtr component) {
        // Call base
        LaurelEye::ISystem<PhysicsBodyComponent>::registerComponent(component);

        // Create Physics Body
        auto newBody = CreateBody(component->GetBodyData());

        // Bind the Body to its Component
        component->SetBodyRef(newBody);
        newBody->BindTransform(component->GetBodyData().transformRef);
    }

    std::shared_ptr<IBody> PhysicsSystem::CreateBody(const PhysicsBodyData& data) {
        return world ? world->CreateBody(data) : nullptr;
    }

    std::shared_ptr<ICollisionShape> PhysicsSystem::CreateShape(const CollisionShapePhys& desc) {
        return world ? world->CreateShape(desc) : nullptr;
    }

    IPhysicsWorld* PhysicsSystem::GetWorld() {
        return world.get();
    }
}
