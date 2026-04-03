#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Physics.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/physics/RigidBodyComponent.h"
#include "LaurelEyeEngine/physics/GhostBodyComponent.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

#include <sol/types.hpp>

namespace LaurelEye::Scripting {
    void Sol2API_Physics::setup(sol::state& lua, EngineContext* ctx) {
        //--Rigid Body Component--
        lua.new_usertype<Physics::RigidBodyComponent>(
            "RigidBodyComponent",
            // Constructors
            sol::no_constructor,
            "applyForce", sol::overload(static_cast<void (Physics::RigidBodyComponent::*)(Vector3)>(&Physics::RigidBodyComponent::ApplyForce),
                [](Physics::RigidBodyComponent& self, float x, float y, float z) {
                self.ApplyForce(Vector3(x, y, z));
            }),
            "applyImpulse", sol::overload(static_cast<void (Physics::RigidBodyComponent::*)(Vector3)>(&Physics::RigidBodyComponent::ApplyImpulse),
                [](Physics::RigidBodyComponent& self, float x, float y, float z) {
                self.ApplyImpulse(Vector3(x, y, z));
            }),

            "getLinearVelocity", &Physics::RigidBodyComponent::GetLinearVelocity,
            "setLinearVelocity", sol::overload(static_cast<void (Physics::RigidBodyComponent::*)(Vector3)>(&Physics::RigidBodyComponent::SetLinearVeloctiy), [](Physics::RigidBodyComponent& self, float x, float y, float z) {
                self.SetLinearVeloctiy(Vector3(x, y, z));
            }),
            "getAngularVelocity", &Physics::RigidBodyComponent::GetAngularVelocity,
            "setAngularVelocity", sol::overload(static_cast<void (Physics::RigidBodyComponent::*)(Vector3)>(&Physics::RigidBodyComponent::SetAngularVelocity), [](Physics::RigidBodyComponent& self, float x, float y, float z) {
                self.SetAngularVelocity(Vector3(x, y, z));
            }),

            // --- Data Access ---
            "getBodyData", &Physics::RigidBodyComponent::GetBodyData,
            "setBodyData", &Physics::RigidBodyComponent::SetBodyData,

            // --- Body Reference Access (optional, may be hidden from scripts) ---
            "getCollider", &Physics::RigidBodyComponent::GetCollider
            /*
            "getBodyRef", &Physics::RigidBodyComponent::GetBodyRef,
            "setBodyRef", &Physics::RigidBodyComponent::SetBodyRef
            */
            );

        // -- Ghost Body Component --
        lua.new_usertype<Physics::GhostBodyComponent>(
            "GhostBodyComponent",
            sol::no_constructor,

            "setLinearVelocity", sol::overload(static_cast<void (Physics::GhostBodyComponent::*)(Vector3)>(&Physics::GhostBodyComponent::SetLinearVeloctiy), [](Physics::GhostBodyComponent& self, float x, float y, float z) {
                self.SetLinearVeloctiy(Vector3(x, y, z));
            }),

            "getLinearVelocity", &Physics::GhostBodyComponent::GetLinearVelocity,

            "setAngularVelocity", sol::overload(static_cast<void (Physics::GhostBodyComponent::*)(Vector3)>(&Physics::GhostBodyComponent::SetAngularVelocity), [](Physics::GhostBodyComponent& self, float x, float y, float z) {
                self.SetAngularVelocity(Vector3(x, y, z));
            }),

            "getAngularVelocity", &Physics::GhostBodyComponent::GetAngularVelocity,

            "applyForce", &Physics::GhostBodyComponent::ApplyForce,

            "getGravityScale", &Physics::GhostBodyComponent::getGravityScale,

            "setGravityScale", &Physics::GhostBodyComponent::setGravityScale,

            "applyImpulse", &Physics::GhostBodyComponent::ApplyImpulse,

            "getBodyData", &Physics::GhostBodyComponent::GetBodyData,

            "setBodyData", &Physics::GhostBodyComponent::SetBodyData,

            // --- Data Access ---
            "getBodyData", &Physics::GhostBodyComponent::GetBodyData,
            "setBodyData", &Physics::GhostBodyComponent::SetBodyData,

            // --- Body Reference Access (optional, may be hidden from scripts) ---
            "getCollider", &Physics::GhostBodyComponent::GetCollider
            );


        // --- Bind PhysicsBodyData ---
        lua.new_usertype<Physics::PhysicsBodyData>(
            "PhysicsBodyData",
            sol::constructors<Physics::PhysicsBodyData()>(),

            // --- Basic ---
            "type", &Physics::PhysicsBodyData::type,
            "shapeDefinition", &Physics::PhysicsBodyData::shapeDefinition,
            "mass", &Physics::PhysicsBodyData::mass,

            // --- Transform Reference ---
            "transformRef", &Physics::PhysicsBodyData::transformRef,

            // --- Physical Properties ---
            "inertia", &Physics::PhysicsBodyData::inertia,
            "centerOfMass", &Physics::PhysicsBodyData::centerOfMass,
            "linearDamping", &Physics::PhysicsBodyData::linearDamping,
            "angularDamping", &Physics::PhysicsBodyData::angularDamping,
            "friction", &Physics::PhysicsBodyData::friction,
            "restitution", &Physics::PhysicsBodyData::restitution,

            // --- Collision Filtering ---
            "collisionGroup", &Physics::PhysicsBodyData::collisionGroup,
            "collisionMask", &Physics::PhysicsBodyData::collisionMask,

            // --- CCD ---
            "useCCD", &Physics::PhysicsBodyData::useCCD,
            "ccdMotionThreshold", &Physics::PhysicsBodyData::ccdMotionThreshold,
            "ccdSweptSphereRadius", &Physics::PhysicsBodyData::ccdSweptSphereRadius,

            // --- Gravity ---
            "gravityScale", &Physics::PhysicsBodyData::gravityScale,

            // --- Static Helpers ---
            "StaticBox", sol::overload([](const Vector3& halfExtents) {
                return Physics::PhysicsBodyData::StaticBox(halfExtents); },
                [](float x, float y, float z) {
                    return Physics::PhysicsBodyData::StaticBox(Vector3(x, y, z)); }),
            "DynamicBox", sol::overload([](const Vector3& halfExtents, float mass) {
                return Physics::PhysicsBodyData::DynamicBox(halfExtents, mass); },
                [](float x, float y, float z, float mass) {
                    return Physics::PhysicsBodyData::DynamicBox(Vector3(x, y, z), mass); }),
            "Sphere", sol::overload([](float radius, float mass, Physics::BodyType type) {
                return Physics::PhysicsBodyData::Sphere(radius, mass, type); },
                [](float radius, float mass) {
                    return Physics::PhysicsBodyData::Sphere(radius, mass); },
                    [](float radius) {
                        return Physics::PhysicsBodyData::Sphere(radius); }),
            "Capsule", sol::overload([](float radius, float height, float mass, Physics::BodyType type) {
                return Physics::PhysicsBodyData::Capsule(radius, height, mass, type); },
                [](float radius, float height, float mass) {
                    return Physics::PhysicsBodyData::Capsule(radius, height, mass); },
                    [](float radius, float height) {
                        return Physics::PhysicsBodyData::Capsule(radius, height); }),
            // --- Trigger / Sensor Helpers ---
            "TriggerBox", sol::overload([](const Vector3& halfExtents) {
                return Physics::PhysicsBodyData::TriggerBox(halfExtents); },
                [](float x, float y, float z) {
                    return Physics::PhysicsBodyData::TriggerBox(Vector3(x, y, z)); }),
            "TriggerSphere", sol::overload([](float radius) {
                return Physics::PhysicsBodyData::TriggerSphere(radius);
            }),
            "SensorCapsule", sol::overload([](float radius, float height) {
                return Physics::PhysicsBodyData::SensorCapsule(radius, height);
            }),
            "TriggerBoxPlayerOnly", sol::overload([](const Vector3& halfExtents) {
                return Physics::PhysicsBodyData::TriggerBoxPlayerOnly(halfExtents); },
                [](float x, float y, float z) {
                    return Physics::PhysicsBodyData::TriggerBoxPlayerOnly(Vector3(x, y, z)); })
        );

        // Bind collision event data
        lua.new_usertype<Physics::CollisionEventData>("CollisionEventData",
                                                      "type", &Physics::CollisionEventData::type,
                                                      "entityA", &Physics::CollisionEventData::entityARef,
                                                      "entityB", &Physics::CollisionEventData::entityBRef,
                                                      "contactPoint", &Physics::CollisionEventData::contactPoint,
                                                      "contactNormal", &Physics::CollisionEventData::contactNormal);

        // Bind layer mask
        // Leaving this here for legacy support
        sol::table Layers = lua["Layers"].get_or_create<sol::table>();
        Layers["Player"] = Physics::CollisionLayer::Player;
        Layers["Enemy"]  = Physics::CollisionLayer::Enemy;
        Layers["World"]  = Physics::CollisionLayer::World;
        Layers["Trigger"]= Physics::CollisionLayer::Trigger;

        // Bind Raycast
        sol::table Physics = lua["Physics"].get_or_create<sol::table>();
        Physics::PhysicsSystem* physics = ctx->getService<Physics::PhysicsSystem>();

        Physics.new_enum(
            "Layers",
            "None", Physics::CollisionLayer::None,
            "Player", Physics::CollisionLayer::Player,
            "Enemy", Physics::CollisionLayer::Enemy,
            "World", Physics::CollisionLayer::World,
            "Projectile", Physics::CollisionLayer::Projectile,
            "Trigger", Physics::CollisionLayer::Trigger,
            "All", Physics::CollisionLayer::All
        );

        Physics.set_function("Raycast",
            [physics](sol::this_state ts,
                            const Vector3& origin,
                            const Vector3& dir,
                            float maxDist,
                            sol::optional<sol::table> opt) -> sol::object {
                assert(!physics->IsSteppingPhysics() && "ERROR::SCRIPTING::RAYCAST::PHYSICS_IS_STEPPING");
                sol::state_view L(ts);
                Physics::RaycastParams params{};
                if ( opt ) {
                    sol::table t = *opt;
                    if ( auto v = t["layerMask"]; v.valid() ) params.layerMask = v.get<uint32_t>();
                }

                Physics::RaycastHit hit = physics->Raycast(
                    Vector3(origin.x, origin.y, origin.z),
                    Vector3(dir.x, dir.y, dir.z),
                    maxDist,
                    params);

                if ( !hit.hit ) {
                    return sol::nil; // idiomatic Lua
                }

                sol::table out = L.create_table();
                Entity* e = hit.entityRef; // your pointer result (nullable)
                if ( !e ) {
                    out["entity"] = sol::nil;
                }
                else {
                    out["entity"] = e;
                }
                out["distance"] = hit.distance;

                // sol::table p = L.create_table();
                // p["x"] = hit.position.x;
                // p["y"] = hit.position.y;
                // p["z"] = hit.position.z;
                // out["position"] = p;
                out["position"] = hit.position;

                // sol::table n = L.create_table();
                // n["x"] = hit.normal.x;
                // n["y"] = hit.normal.y;
                // n["z"] = hit.normal.z;
                // out["normal"] = n;
                out["normal"] = hit.normal;

                return sol::make_object(L, out);
            });
    }
} // namespace LaurelEye::Scripting
