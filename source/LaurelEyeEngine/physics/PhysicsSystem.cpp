#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletWorld.h"
#include "LaurelEyeEngine/events/CollisionEvent.h"
#include "LaurelEyeEngine/events/EventManager.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"

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

    void PhysicsSystem::deregisterComponent(const ComponentPtr component) {
        if ( !component ) return;

        // Get body ref
        auto bodyRef = component->GetBodyRef();
        if ( bodyRef ) {
            // Unbind from component and transform
            bodyRef->BindPhysicsBodyComponent(nullptr);
            bodyRef->BindTransform(nullptr);

            // Remove from world
            if ( world ) {
                world->RemoveBody(bodyRef);
            }

            // Reset the component's body reference
            component->SetBodyRef(nullptr);
        }

        // Finally remove from active list
        ISystem<PhysicsBodyComponent>::deregisterComponent(component);
    }

    void PhysicsSystem::registerCollisionEnterListeners() {
        auto* eventManager = context->getService<EventManager>();
        assert(eventManager && "PhysicsSystem: No EventManager in EngineContext");
        // Add the scripting event bindings
        enterListener = eventManager->addListener<CollisionEnterEvent>(
            [&](const CollisionEnterEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityARef && data.entityBRef);

                // Forward to Lua if either entity has a script
                if ( auto* scriptA = data.entityARef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onCollisionEnter(data);
                    }
                }

                if ( auto* scriptB = data.entityBRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptB->getScriptInstance() ) {
                        instance->onCollisionEnter(data);
                    }
                }

                std::cout << "[Enter] " << data << std::endl;
            });

        stayListener = eventManager->addListener<CollisionStayEvent>(
            [&](const CollisionStayEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityARef && data.entityBRef);

                if ( auto* scriptA = data.entityARef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onCollisionStay(data);
                    }
                }

                if ( auto* scriptB = data.entityBRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptB->getScriptInstance() ) {
                        instance->onCollisionStay(data);
                    }
                }
            });

        exitListener = eventManager->addListener<CollisionExitEvent>(
            [&](const CollisionExitEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityARef && data.entityBRef);

                if ( auto* scriptA = data.entityARef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onCollisionExit(data);
                    }
                };

                if ( auto* scriptB = data.entityBRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptB->getScriptInstance() ) {
                        instance->onCollisionExit(data);
                    }
                }

                std::cout << "[Exit] " << data << std::endl;
            });
    }

    void PhysicsSystem::deregisterCollisionEnterListeners() {
        auto* eventManager = context->getService<EventManager>();
        assert(eventManager && "PhysicsSystem: No EventManager in EngineContext");
        eventManager->removeListener<CollisionEnterEvent>(enterListener);
        eventManager->removeListener<CollisionStayEvent>(stayListener);
        eventManager->removeListener<CollisionExitEvent>(exitListener);
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

    void PhysicsSystem::populateWireFrameCommands(std::vector<Debug::DebugDrawCommand>& commands) const {

        for ( PhysicsBodyComponent* const pbc : components ) {
            std::shared_ptr<IBody> body = pbc->GetBodyRef();
            if ( !body ) continue;

            LaurelEye::TransformComponent* transform = body->GetBoundTransform();
            if ( !transform ) continue;

            const Vector3 pos = transform->getWorldPosition();
            const Quaternion rot = transform->getWorldRotation();
            const Vector3 scale = transform->getWorldScale();

            PhysicsBodyData bodyData = pbc->GetBodyData();
            CollisionShapePhys shapeDef = bodyData.shapeDefinition;

            // --- Shape wireframe ---
            Debug::DebugDrawCommand cmd;
            cmd.position = pos;
            cmd.rotation = rot;
            cmd.color = {0.0f, 1.0f, 0.0f}; // Green for collider outlines

            switch ( shapeDef.type ) {
                case CollisionShapePhys::ShapeType::Box: {
                    cmd.type = Debug::DebugDrawType::Box;
                    cmd.size = shapeDef.size * scale; // element-wise scaling
                    break;
                }

                case CollisionShapePhys::ShapeType::Sphere: {
                    cmd.type = Debug::DebugDrawType::Sphere;
                    float avgScale = (scale.x + scale.y + scale.z) / 3.0f;
                    cmd.radius = shapeDef.radius * avgScale;
                    break;
                }

                case CollisionShapePhys::ShapeType::Capsule: {
                    cmd.type = Debug::DebugDrawType::Capsule;
                    float radialScale = (scale.x + scale.z) * 0.5f; // average XZ for radius
                    cmd.radius = shapeDef.radius * radialScale;
                    cmd.size = {0.0f, shapeDef.height * scale.y, 0.0f};
                    break;
                }
            }

            commands.push_back(cmd);
            //Do This Next
            /*
            // --- Velocity Vector ---
            const Vector3 velocity = body->GetLinearVelocity();
            if ( velocity.magnitudSquared() > 0.0001f ) {
                Debug::DebugDrawCommand velCmd;
                velCmd.type = Debug::DebugDrawType::VelocityVector;
                velCmd.position = pos;
                velCmd.direction = velocity.normalized();
                velCmd.size = {velocity.magnitude(), 0.0f, 0.0f};
                velCmd.color = {1.0f, 0.0f, 0.0f}; // Red for velocity
                commands.push_back(velCmd);
            }

            // --- Force Vector (if available) ---
            if ( body->HasAppliedForces() ) {
                Vector3 totalForce = body->GetAccumulatedForce();
                if ( totalForce.magnitudSquared() > 0.0001f ) {
                    Debug::DebugDrawCommand forceCmd;
                    forceCmd.type = Debug::DebugDrawType::VelocityVector; // reuse arrow
                    forceCmd.position = pos;
                    forceCmd.direction = totalForce.normalized();
                    forceCmd.size = {totalForce.magnitude() * 0.01f, 0.0f, 0.0f}; // scale for visibility
                    forceCmd.color = {1.0f, 1.0f, 0.0f};                 // Yellow for forces
                    commands.push_back(forceCmd);
                }
            }
            */
            //Do This Last
            /*
            // --- Surface Normals  ---
            if ( body->HasSurfaceNormals() ) {
                const auto& normals = body->GetSurfaceNormals();
                for ( const auto& n : normals ) {
                    Debug::DebugDrawCommand normalCmd;
                    normalCmd.type = Debug::DebugDrawType::Normal;
                    normalCmd.position = n.origin;
                    normalCmd.direction = n.direction;
                    normalCmd.size = {0.5f, 0, 0};        // length of normal
                    normalCmd.color = {0.0f, 0.0f, 1.0f}; // Blue for normals
                    commands.push_back(normalCmd);
                }
            }
            */
        }
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
