/// @file   IBody.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief Interface for physics bodies in the simulation.
/// Represents objects with forces, velocity, and transform binding, independent of backend.

#pragma once

#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
#include "LaurelEyeEngine/physics/interfaces/ICollider.h"

#include <atomic>

namespace LaurelEye::Physics {

    /// @brief Interface for physics bodies managed by a physics world.
    /// An IBody represents a single object in the physics simulation, which may be static,
    /// dynamic, or kinematic depending on its configuration
    class RigidBodyComponent;

    class IRigidBody : public ICollider {
    public:
        IRigidBody()
            : bodyId(GenerateUniqueID()){}
        /// @brief Virtual destructor for safe cleanup of derived classes.
        virtual ~IRigidBody() = default;

        /// @brief Set the transform of this body.
        /// @param t The new position and orientation.
        virtual void BindTransform(LaurelEye::TransformComponent* t) override = 0;
        /// @brief Get the current transform of this body.
        /// @return The current position and orientation.
        virtual LaurelEye::TransformComponent* GetBoundTransform() const override = 0;

        /// @brief Send Transform data to the Physics System
        virtual void pushTransformToPhysics() override = 0;
        /// @brief Update transform data from the Physics System
        virtual void updateTransformFromPhysics() override = 0;

        /// @brief
        virtual void* GetUserData() const override = 0;
        /// @brief
        /// @param ptr
        virtual void SetUserData(void* ptr) override = 0;

        /// @brief Apply a continuous force to this body.
        /// @param f The force vector to apply.
        virtual void ApplyForce(const Vector3& f) = 0;
        /// @brief Apply an instantaneous impulse to this body.
        /// @param f The impulse vector to apply.
        virtual void ApplyImpulse(const Vector3& f) = 0;

        /// @brief Set the linear velocity of this body.
        /// @param v The velocity vector.
        virtual void SetLinearVelocity(const Vector3& v) = 0;
        /// @brief Get the current linear velocity of this body.
        /// @return The velocity vector.
        virtual Vector3 GetLinearVelocity() const = 0;

        /// @brief Set the linear velocity of this body.
        /// @param v The velocity vector.
        virtual void SetAngularVelocity(const Vector3& v) = 0;
        /// @brief Get the current linear velocity of this body.
        /// @return The velocity vector.
        virtual Vector3 GetAngularVelocity() const = 0;

        /// @brief Set the mass of this body.
        /// @param m The mass value.
        virtual void SetMass(float m) = 0;
        /// @brief Get the mass of this body.
        /// @return The mass value.
        virtual float GetMass() const = 0;

        virtual Vector3 GetGravityScale() const = 0;

        virtual void SetGravityScale(Vector3 nv) = 0;

        //virtual std::any GetInternal() = 0; Potential Optimization, Ignore for now

        unsigned int GetBodyID() const override { return bodyId; }

    private:
        unsigned int bodyId;

    };

} // namespace LaurelEye::Physics
