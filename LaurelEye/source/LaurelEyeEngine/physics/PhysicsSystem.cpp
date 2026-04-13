#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/logging/EngineLog.h"
#include "LaurelEyeEngine/physics/interfaces/ICollider.h"
#include "LaurelEyeEngine/physics/interfaces/IRigidBody.h"
#include "LaurelEyeEngine/physics/interfaces/IGhostBody.h"
#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletWorld.h"
#include "LaurelEyeEngine/events/CollisionEvent.h"
#include "LaurelEyeEngine/events/TriggerEvent.h"
#include "LaurelEyeEngine/events/EventManager.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/physics/interfaces/IPhysicsWorld.h"
#include <cassert>

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
            comp->SyncToPhysics();
        }

        isSteppingPhysics = true;
        if ( world ) world->StepSimulation(dt, 4, 1.0f/60.0f * context->time().timeScale);
        isSteppingPhysics = false;

        //Sync Physics to Transforms
        for ( auto* comp : components ) {
            comp->SyncFromPhysics();
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

        LaurelEye::ISystem<PhysicsBodyBaseComponent>::registerComponent(component);

        //Create in world
        component->CreateInWorld(*this);

    }

    void PhysicsSystem::deregisterComponent(const ComponentPtr component) {

        if ( !component ) return;

        component->RemoveFromWorld(*this);

        LaurelEye::ISystem<PhysicsBodyBaseComponent>::deregisterComponent(component);

    }

    void PhysicsSystem::registerCollisionEnterListeners() {
        auto* eventManager = context->getService<EventManager>();
        assert(eventManager && "PhysicsSystem: No EventManager in EngineContext");
        // Add the scripting event bindings
        enterListener = eventManager->addListener<CollisionEnterEvent>(
            [&](const CollisionEnterEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityARef && data.entityBRef);

                std::cout << "[EnterCollisioner] Listener is Run" << std::endl;
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

        enterTriggerListener = eventManager->addListener<TriggerEnterEvent>(
            [&](const TriggerEnterEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityARef && data.entityBRef);

                std::cout << "[EnterTrigger] Listener is Run" << std::endl;
                if ( auto* scriptA = data.entityARef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onTriggerEnter(data);
                    }
                };

                if ( auto* scriptB = data.entityBRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptB->getScriptInstance() ) {
                        instance->onTriggerEnter(data);
                    }
                }

            }
        );

        stayTriggerListener = eventManager->addListener<TriggerStayEvent>(
            [&](const TriggerStayEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityARef && data.entityBRef);

                if ( auto* scriptA = data.entityARef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onTriggerStay(data);
                    }
                };

                if ( auto* scriptB = data.entityBRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptB->getScriptInstance() ) {
                        instance->onTriggerStay(data);
                    }
                }

                //std::cout << "[StayTrigger] " << data << std::endl;
            }
        );

        exitTriggerListener = eventManager->addListener<TriggerExitEvent>(
            [&](const TriggerExitEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityARef && data.entityBRef);

                if ( auto* scriptA = data.entityARef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onTriggerExit(data);
                    }
                };

                if ( auto* scriptB = data.entityBRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptB->getScriptInstance() ) {
                        instance->onTriggerExit(data);
                    }
                }

                //std::cout << "[ExitTrigger] " << data << std::endl;
            }
        );
    }

    void PhysicsSystem::deregisterCollisionEnterListeners() {
        auto* eventManager = context->getService<EventManager>();
        assert(eventManager && "PhysicsSystem: No EventManager in EngineContext");
        eventManager->removeListener<CollisionEnterEvent>(enterListener);
        eventManager->removeListener<CollisionStayEvent>(stayListener);
        eventManager->removeListener<CollisionExitEvent>(exitListener);
        eventManager->removeListener<TriggerEnterEvent>(enterTriggerListener);
        eventManager->removeListener<TriggerStayEvent>(stayTriggerListener);
        eventManager->removeListener<TriggerExitEvent>(exitTriggerListener);
    }

    std::shared_ptr<ICollider> PhysicsSystem::CreateCollider(const PhysicsBodyData& data) {

        if ( !world ) return nullptr;

        //TODO: Creation Logic
        switch ( data.type ) {
        case BodyType::Static:
            return std::static_pointer_cast<ICollider>(world->CreateRigidBody(data));
            break;
        case BodyType::Dynamic:
            return std::static_pointer_cast<ICollider>(world->CreateRigidBody(data));
            break;
        case BodyType::Kinematic:
            return std::static_pointer_cast<ICollider>(world->CreateRigidBody(data));
            break;
        case BodyType::Ghost:
            return std::static_pointer_cast<ICollider>(world->CreateGhostBody(data));
            break;
        default:
            return nullptr;
            break;
        }

    }

    std::shared_ptr<ICollisionShape> PhysicsSystem::CreateShape(const CollisionShapePhys& desc) {
        return world ? world->CreateShape(desc) : nullptr;
    }

    IPhysicsWorld* PhysicsSystem::GetWorld() {
        return world.get();
    }

    void PhysicsSystem::dispatchCollisionEvents() const {
        if ( !context ) {
            //There is no Context and Therefore cannot generate Events
            return;
        }
        const auto& events = collisionManager.getEvents();
        auto* eventManager = context->getService<EventManager>();
        if ( !eventManager )
            throw std::runtime_error("No Event Manager");

        for ( const auto& evt : events ) {
            //TODO: Use the Event Manager Sigleton or System Coordinator
            // to Dispatch the events

            //create collision event
            //std::shared_ptr<CollisionEvent> eventPtr = nullptr;

            switch ( evt.interaction ) {
            case CollisionEventData::Interaction::Collision:

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

                break;
            case CollisionEventData::Interaction::Trigger:

                switch ( evt.type ) {
                case CollisionEventData::Type::Enter: {
                    TriggerEnterEvent enterEvent(evt);
                    eventManager->broadcastEvent(enterEvent); // pass by const reference
                    break;
                }
                case CollisionEventData::Type::Stay: {
                    TriggerStayEvent stayEvent(evt);
                    eventManager->broadcastEvent(stayEvent); // pass by const reference
                    break;
                }
                case CollisionEventData::Type::Exit: {
                    TriggerExitEvent exitEvent(evt);
                    eventManager->broadcastEvent(exitEvent); // pass by const reference
                    break;
                }
                default:
                    throw std::runtime_error("Unknown collision event type!");
                }

                break;

            case CollisionEventData::Interaction::ERROR:
                throw std::runtime_error("ERROR Unknown collision Interaction event type!");
                break;
            default:
                throw std::runtime_error("Unknown collision Interaction event type!");
                break;
            }

            //std::cout << evt << std::endl;
        }
    }
}
