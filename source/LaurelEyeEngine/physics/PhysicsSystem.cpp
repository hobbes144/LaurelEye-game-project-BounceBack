#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletWorld.h"
#include "LaurelEyeEngine/events/CollisionEvent.h"
#include "LaurelEyeEngine/events/EventManager.h"

namespace LaurelEye::Physics {
    PhysicsSystem::PhysicsSystem(PhysicsSystemType type) {
        systemType = type;
    }

    void PhysicsSystem::initialize() {
        //Create World
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

        //Collision Manager
        collisionManager.beginFrame();
        world->GatherCollisions(collisionManager);
        collisionManager.endFrame();
        dispatchCollisionEvents();
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
        newBody->BindPhysicsBodyComponent(component);
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

    void PhysicsSystem::dispatchCollisionEvents() {
        const auto& events = collisionManager.getEvents();
        if ( !context ) {
            //There is no Context and Therefore cannot generate Events
            return;
        }
        auto* eventManager = context->getService<EventManager>();
        if ( !eventManager )
            throw std::runtime_error("No Event Manager");

        for ( const auto& evt : events ) {
            //TODO: Use the Event Manager Sigleton or System Coordinator
            // to Dispatch the events

            //create collision event
            std::shared_ptr<CollisionEvent> eventPtr = nullptr;

            switch ( evt.type ) {
            case CollisionEventData::Type::Enter: {
                CollisionEnterEvent enterEvent(evt);
                eventManager->broadcastEvent(enterEvent); // pass by const reference
                break;
            }
            case CollisionEventData::Type::Stay: {
                CollisionStayEvent stayEvent(evt);
                eventManager->broadcastEvent(stayEvent); // pass by const reference
                break;
            }
            case CollisionEventData::Type::Exit: {
                CollisionExitEvent exitEvent(evt);
                eventManager->broadcastEvent(exitEvent); // pass by const reference
                break;
            }
            default:
                throw std::runtime_error("Unknown collision event type!");
            }

            //std::cout << evt << std::endl;
        }
    }

}
