/// @file   PhysicsTypes.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief  Defines core physics data types used by the physics system, including
///         body types, collision shape descriptors, and body configuration data.
///         Provides helper factory methods for common body definitions.

#pragma once

#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Quaternion.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"

namespace LaurelEye::Physics {

    /// @brief PhysicsId is shorthand for std::uint32_t
    using PhysicsId = std::uint32_t;

    enum class Axis {
        X,Y,Z
    };

    /// @brief The type of body
    enum class BodyType {
        Static, Dynamic, Kinematic, Ghost
    };

    enum CollisionLayer : uint32_t {
        None = 0,
        Player = 1 << 0,
        Enemy = 1 << 1,
        World = 1 << 2,
        Projectile = 1 << 3,
        Trigger = 1 << 4,
        Sensor = 1 << 5,
        All = 0xFFFFFFFFu
    };

    /// @brief Definition of a Physics Shape, Used to build shapes and bodies
    struct CollisionShapePhys {
        enum class ShapeType {Box, Sphere, Capsule, Mesh } type = ShapeType::Box;
        Vector3 size = Vector3(1, 1, 1);
        float radius = 0.5f;
        float height = 1.0f;
    };

    struct PhysicsBodyData {
        //Basic Stuff
        BodyType type = BodyType::Dynamic;
        CollisionShapePhys shapeDefinition = {};
        float mass = 1.0f;
        //Transform
        LaurelEye::TransformComponent* transformRef = nullptr;
        // Physical properties
        Vector3 centerOfMass = {0, 0, 0};
        Vector3 linearVelocity = {0, 0, 0};
        Vector3 angularVelocity = {0, 0, 0};
        Vector3 inertia = {0, 0, 0};
        float linearDamping = 0.0f;
        float angularDamping = 0.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        //Translation Freezing
        Vector3 freezeTranslation = {false, false, false};
        //Rotation Freezing
        Vector3 freezeRotation = {false, false, false};

        // Collision filtering
        uint32_t collisionGroup = CollisionLayer::World;
        uint32_t collisionMask = CollisionLayer::All;
        // Continuous collision detection
        bool useCCD = false;
        float ccdMotionThreshold = 0.0f;
        float ccdSweptSphereRadius = 0.0f;
        // Gravity
        float gravityScale = 1.0f;

        // ---------- User-Friendly Helpers ----------

        static PhysicsBodyData StaticBox(const Vector3& halfExtents) {
            PhysicsBodyData d;
            d.type = BodyType::Static;
            d.shapeDefinition.type = CollisionShapePhys::ShapeType::Box;
            d.shapeDefinition.size = halfExtents;
            d.mass = 0.0f;
            return d;
        }

        static PhysicsBodyData DynamicBox(const Vector3& halfExtents, float mass = 1.0f) {
            PhysicsBodyData d;
            d.type = BodyType::Dynamic;
            d.shapeDefinition.type = CollisionShapePhys::ShapeType::Box;
            d.shapeDefinition.size = halfExtents;
            d.mass = mass;
            d.useCCD = true;
            d.ccdMotionThreshold = 0.01f;
            d.ccdSweptSphereRadius = std::max(std::max(halfExtents.x, halfExtents.y), halfExtents.z);
            return d;
        }

        static PhysicsBodyData Sphere(float radius, float mass = 1.0f, BodyType type = BodyType::Dynamic) {
            PhysicsBodyData d;
            d.type = type;
            d.shapeDefinition.type = CollisionShapePhys::ShapeType::Sphere;
            d.shapeDefinition.radius = radius;
            d.mass = (type == BodyType::Static ? 0.0f : mass);
            return d;
        }

        static PhysicsBodyData Capsule(float radius, float height, float mass = 1.0f, BodyType type = BodyType::Dynamic) {
            PhysicsBodyData d;
            d.type = type;
            d.shapeDefinition.type = CollisionShapePhys::ShapeType::Capsule;
            d.shapeDefinition.radius = radius;
            d.shapeDefinition.height = height;
            d.mass = (type == BodyType::Static ? 0.0f : mass);
            return d;
        }

        static PhysicsBodyData TriggerBox(const Vector3& halfExtents) {
            PhysicsBodyData d;
            d.type = BodyType::Static;
            d.shapeDefinition.type = CollisionShapePhys::ShapeType::Box;
            d.shapeDefinition.size = halfExtents;
            d.mass = 0.0f;

            d.collisionGroup = CollisionLayer::Trigger;
            d.collisionMask = CollisionLayer::Player |
                              CollisionLayer::Enemy |
                              CollisionLayer::Projectile;

            d.friction = 0.0f;
            d.restitution = 0.0f;
            return d;
        }

        static PhysicsBodyData TriggerSphere(float radius) {
            PhysicsBodyData d;
            d.type = BodyType::Static;
            d.shapeDefinition.type = CollisionShapePhys::ShapeType::Sphere;
            d.shapeDefinition.radius = radius;
            d.mass = 0.0f;

            d.collisionGroup = CollisionLayer::Trigger;
            d.collisionMask = CollisionLayer::Player;

            d.friction = 0.0f;
            d.restitution = 0.0f;
            return d;
        }

        static PhysicsBodyData SensorCapsule(float radius, float height) {
            PhysicsBodyData d;
            d.type = BodyType::Kinematic;
            d.shapeDefinition.type = CollisionShapePhys::ShapeType::Capsule;
            d.shapeDefinition.radius = radius;
            d.shapeDefinition.height = height;
            d.mass = 0.0f;

            d.collisionGroup = CollisionLayer::Sensor;
            d.collisionMask = CollisionLayer::Player | CollisionLayer::Enemy;

            d.friction = 0.0f;
            d.restitution = 0.0f;
            return d;
        }

        static PhysicsBodyData TriggerBoxPlayerOnly(const Vector3& halfExtents) {
            auto d = TriggerBox(halfExtents);
            d.collisionMask = CollisionLayer::Player;
            return d;
        }

    };

    /// @class RaycastHit
    /// @brief Results of a raycast for each hit.
    ///
    struct RaycastHit {
        bool hit = false;
        float distance = 0.0f;
        Vector3 position{};
        Vector3 normal{};
        // unsigned int entityID = 0;
        Entity* entityRef = nullptr;
        // uint32_t colliderId = 0; // for sub-colliders, unimplemented
    };

    struct RaycastParams {
        uint32_t layerMask = CollisionLayer::All;
        bool closest = true;      // closest hit vs all hits
        // unsigned int ingoreID = 0; // Ignore certain obj
        // Entity* ignoreRef = nullptr;
        // bool hitTriggers = false; // for trigger volumes, unimplemented
    };
}
