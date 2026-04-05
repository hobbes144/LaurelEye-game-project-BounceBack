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

#include <cstdint>
#include <memory>

#include "LaurelEyeEngine/physics/interfaces/IPhysicsWorld.h"
#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
#include "LaurelEyeEngine/physics/CollisionManager.h"
#include "LaurelEyeEngine/debugDraw/DebugDrawSystem.h"
#include "LaurelEyeEngine/ecs/ISystem.h"

//Forward Declaration
namespace LaurelEye::Debug {
    struct DebugDrawCommand;
}

namespace LaurelEye::Physics {

    /// @brief Defines physics backends for the PhysicsSystem.
    enum class PhysicsSystemType {
        Bullet
    };

    /// @brief This system manages the lifecycle of the physics simulation
    /// (initialize, update, shutdown), provides creation methods for collision
    /// shapes and physics bodies, and acts as a bridge between ECS and Physics Backend
    class PhysicsSystem : public ISystem<PhysicsBodyBaseComponent> {
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

        void registerCollisionEnterListeners();
        void deregisterCollisionEnterListeners();

        /// @brief Create a physics body and add it to the world
        /// @param type The body type (dynamic, static, kinematic)
        /// @param shapeDesc The collision shape descriptor for the body
        /// @param transform Initial transform
        /// @return A shared pointer to the created IBody
        std::shared_ptr<ICollider> CreateCollider(const PhysicsBodyData& data);

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

        void populateWireFrameCommands(std::vector<Debug::DebugDrawCommand>& commands) const ;

        // Physics Query API

        /// @brief Raycast query.
        ///
        /// @param origin Ray origin.
        /// @param direction Ray direction.
        /// @param maxDistance Maximum ray distance.
        /// @param params Raycast parameters.
        /// @return Raycast hit.
        RaycastHit Raycast(const Vector3& origin,
                           const Vector3& direction,
                           float maxDistance,
                           const RaycastParams& params) const;

        /// @brief Raycast query.
        ///
        /// @param from Ray start.
        /// @param to Ray to.
        /// @param params Raycast parameters.
        /// @return Raycast hit.
        RaycastHit Raycast(const Vector3& from,
                           const Vector3& to,
                           const RaycastParams& params) const;

        // /// @brief Helper function to return if any RayHit occurred.
        // ///
        // /// @return True if hit.
        // // bool RaycastAny(...);
        // /// @brief Helper function to get all hit objects from Raycast.
        // ///
        // /// @return Vector of Raycast hits.
        // std::vector<RaycastHit> RaycastAll(...);

        // /// @brief Spherecast query.
        // ///
        // /// @param origin Sphere origin.
        // /// @param radius Sphere radius.
        // /// @param params Spherecast parameters.
        // /// @return Spherecast hit.
        // RaycastHit Spherecast(const Vector3& origin,
        //                    float radius,
        //                    const RaycastParams& params);

        bool IsSteppingPhysics() const { return isSteppingPhysics; };

    private:

        /// @brief Selected physics backend
        PhysicsSystemType systemType;
        /// @brief Unique pointer to world instance
        std::unique_ptr<IPhysicsWorld> world;
        CollisionManager collisionManager;

        bool isSteppingPhysics = false;

        uint32_t enterListener;
        uint32_t stayListener;
        uint32_t exitListener;

        uint32_t enterTriggerListener;
        uint32_t stayTriggerListener;
        uint32_t exitTriggerListener;

        void dispatchCollisionEvents() const;
        //void gatherTriggerEvents();

    };

}
