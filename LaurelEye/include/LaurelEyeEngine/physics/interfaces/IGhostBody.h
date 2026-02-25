///
/// @file   IGhostBody.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   02-04-2026
/// @brief
///
///
///

#pragma once

#include "LaurelEyeEngine/physics/interfaces/ICollider.h"
#include <vector>

namespace LaurelEye::Physics {

    class IGhostBody : public ICollider {

    public:
        IGhostBody()
            : bodyId(GenerateUniqueID()){}
        virtual ~IGhostBody() override = default;

        // Transform binding
        virtual void BindTransform(TransformComponent* t) override = 0;
        virtual LaurelEye::TransformComponent* GetBoundTransform() const override = 0;

        // Manual sync
        virtual void pushTransformToPhysics() override = 0;
        virtual void updateTransformFromPhysics() override = 0;

        //Overlaps
        virtual std::vector<ICollider*> GetOverlaps() const = 0;
        virtual int GetNumberOverlappingObjects() const = 0;

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

        // Collider Override
        unsigned int GetBodyID() const override { return bodyId; }

    private:
        unsigned int bodyId;
    };


}

