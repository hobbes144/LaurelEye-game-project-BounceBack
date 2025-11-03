/// @file   PhysicsSystem.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief ECS-integrated physics system.
/// Wraps a physics backend (e.g., Bullet),
/// manages simulation lifecycle, and creates
/// physics bodies and collision shapes.

#pragma once

#include <memory>

#include "LaurelEyeEngine/physics/interfaces/IPhysicsWorld.h"
#include "LaurelEyeEngine/physics/CollisionManager.h"
#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"
#include "LaurelEyeEngine/ecs/ISystem.h"

namespace LaurelEye::Physics {

    /// @brief Defines physics backends for the PhysicsSystem.
    enum class PhysicsSystemType {
        Bullet
    };

    /// @brief This system manages the lifecycle of the physics simulation
    /// (initialize, update, shutdown), provides creation methods for collision 
    /// shapes and physics bodies, and acts as a bridge between ECS and Physics Backend
    class PhysicsSystem : public ISystem<PhysicsBodyComponent> {
    public:
        /// @brief Construct a new physics system
        /// @param type Physics backend type to be used (default: Bullet)
        PhysicsSystem(PhysicsSystemType type = PhysicsSystemType::Bullet);

        /// @brief Initialize the physics system and create the underlying world.
        void initialize() override;
        /// @brief Updates physics world by advancing the simulation
        /// @param dt Delta time (in seconds) since the last update.
        void update(float dt) override;
        /// @brief Shutdown physics system
        void shutdown() override;

        void registerComponent(const ComponentPtr component) override;
        void deregisterComponent(const ComponentPtr component) override;
        /// @brief Create a physics body and add it to the world
        /// @param type The body type (dynamic, static, kinematic)
        /// @param shapeDesc The collision shape descriptor for the body
        /// @param transform Initial transform 
        /// @return A shared pointer to the created IBody
        std::shared_ptr<IBody> CreateBody(const PhysicsBodyData& data);

        /// @brief Create a collision shape based on the provided descriptor
        /// @param desc Collision shape descriptor
        /// @return A shared pointer to the created ICollisionShape
        std::shared_ptr<ICollisionShape> CreateShape(const CollisionShapePhys& desc);

        /// @brief Get a pointer to the active physics world
        /// @return A raw pointer to the IPhysicsWorld (owned internally)
        IPhysicsWorld* GetWorld();
   
        CollisionManager& GetCollisionManager() {
            return collisionManager;
        }

    private:
        /// @brief Selected physics backend
        PhysicsSystemType systemType;
        /// @brief Unique pointer to world instance
        std::unique_ptr<IPhysicsWorld> world;
        CollisionManager collisionManager;

        void dispatchCollisionEvents();

    };

}
