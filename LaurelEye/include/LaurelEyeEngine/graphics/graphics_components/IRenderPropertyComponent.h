/// @file   IRenderPropertyComponent.h
/// @author Nick Shaw
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10-07-2025
/// @brief  Base interface for all render-property components.
///
/// This file defines the `IRenderPropertyComponent` class, which serves as
/// the abstract base for all rendering property components such as
/// lights, cameras, and environment settings. These components do not
/// directly render geometry but instead influence how rendering occurs.
///

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/IRenderComponent.h"

namespace LaurelEye::Graphics {

    /// @class IRenderPropertyComponent
    /// @brief Base interface for rendering property components.
    ///
    /// Inherits from `IRenderComponent` and provides a base type for
    /// components that affect global or environmental rendering properties,
    /// such as lighting, camera view, or post-processing.
    ///
    /// Unlike `IRenderableComponent`, property components do not hold meshes
    /// or materials, but instead modify rendering state or context.
    class IRenderPropertyComponent : public IRenderComponent {

    public:
        /// @brief Constructs a render-property component with a specific type.
        /// @param type The render component classification (e.g., `PropertyCamera`, `PropertyLight`).
        explicit IRenderPropertyComponent(RenderComponentType type)
            : IRenderComponent(type) {}

        /// @brief Virtual destructor.
        virtual ~IRenderPropertyComponent() = default;
    };

}
