///
/// @file   ICollider.h
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

#include "LaurelEyeEngine/transform/TransformComponent.h"
#include <atomic>
#include <memory>

namespace LaurelEye::Physics {

    class PhysicsBodyBaseComponent;
    /// @brief This Class represents collision Objects.
    /// It binds to a transform and communicates it with the Physics System
    class ICollider {
    public:
        virtual ~ICollider() = default;

        virtual void BindTransform(TransformComponent* t) = 0;
        virtual TransformComponent* GetBoundTransform() const = 0;

        virtual void pushTransformToPhysics() = 0;
        virtual void updateTransformFromPhysics() = 0;

        virtual void* GetUserData() const = 0;
        virtual void SetUserData(void* ptr) = 0;

        virtual unsigned int GetBodyID() const = 0;

    protected:
        std::shared_ptr<PhysicsBodyBaseComponent> boundPhysicsComponent;

        static unsigned int GenerateUniqueID() {
            static std::atomic<unsigned int> pNextID{1};
            return pNextID++;
        }

    };

}
