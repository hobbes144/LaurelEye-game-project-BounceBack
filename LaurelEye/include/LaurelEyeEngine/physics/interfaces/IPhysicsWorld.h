/// @file   IPhysicsWorld.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief Defines the interface for a physics simulation world.
/// Provides an abstraction layer for advancing simulation, creating bodies,
/// and managing collision shapes, independent of the underlying physics engine.

#pragma once

#include <memory>

#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"
#include "LaurelEyeEngine/physics/interfaces/IBody.h"
#include "LaurelEyeEngine/physics/CollisionManager.h"

namespace LaurelEye::Physics {
    class IBody;
    class ICollisionShape;

    /// @brief Interface for a physics simulation world.
    /// Provides a generic abstraction layer for physics engines.
    /// Implementaions handle simulation, body creation, and shape managment,
    /// while exposing a unified API to the rest of the engine
    class IPhysicsWorld {
    public:
        /// @brief Virtual destructor for safe cleanup of derived worlds.
        virtual ~IPhysicsWorld() = default;

        /// @brief Advances the simulation state by the given delta time.
        /// @param dt Delta time in seconds since the last update.
        virtual void StepSimulation(float dt) = 0;

        /// @brief Create a new physics body.
        /// @param type The type of body (static, dynamic, kinematic).
        /// @param shapeDesc Description of the collision shape to attach.
        /// @param start Initial transform for the body.
        /// @return A shared pointer to the created body.
        virtual std::shared_ptr<IBody> CreateBody(const PhysicsBodyData& data) = 0;

        virtual void RemoveBody(std::shared_ptr<IBody> body) = 0;

        /// @brief Create a new collision shape.
        /// @param csPhys Collision shape description.
        /// @return A shared pointer to the created collision shape.
        virtual std::shared_ptr<ICollisionShape> CreateShape(
            const CollisionShapePhys& csPhys) = 0;

        virtual void GatherCollisions(CollisionManager& cm) = 0;

    protected:
        //TODO: Track Physics Bodies in a more efficient manner

        /// @brief Active physics bodies tracked by this world.
        std::vector<std::shared_ptr<IBody>> bodies;
        /// @brief Collision shapes tracked by this world.
        std::vector<std::shared_ptr<ICollisionShape>> shapes;
    };
}
