/// @file   ICollisionShape.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief Interface for collision shapes in the physics system.
/// Defines geometric properties of objects and abstracts underlying physics engine shapes.

#pragma once

#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"

namespace LaurelEye::Physics {

    /// @brief Interface for collision shapes used in the physics system.
    /// Shapes define the geometric properties of physics objects,
    /// Implementation wrap external-libraray-specific collision shape while exposing a generic API
    class ICollisionShape {
    public:
        /// @brief Virtual destructor for safe cleanup of derived shapes.
        virtual ~ICollisionShape() = default;

        /// @brief Retrieve the stored shape description.
        /// Provides access to the metadata describing this shape, such astype, 
        /// size, radius, or height depending on the shape type.
        /// @return The collision shape description.
        virtual CollisionShapePhys GetDescription() const = 0;
    };

}
