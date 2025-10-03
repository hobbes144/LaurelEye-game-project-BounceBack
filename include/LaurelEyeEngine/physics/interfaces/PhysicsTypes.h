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

    /// @brief The type of body
    enum class BodyType {
        Static, Dynamic, Kinematic
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
        Vector3 inertia = {0, 0, 0};
        Vector3 centerOfMass = {0, 0, 0};
        float linearDamping = 0.0f;
        float angularDamping = 0.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        // Collision filtering
        uint32_t collisionGroup = 1;
        uint32_t collisionMask = 0xFFFFFFFF;
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
    };

}
