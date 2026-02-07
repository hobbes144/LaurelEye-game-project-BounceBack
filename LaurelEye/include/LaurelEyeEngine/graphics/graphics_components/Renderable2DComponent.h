/// @file   Renderable2DComponent.h
/// @author Nick Shaw
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10-07-2025
/// @brief  2D renderable component for the LaurelEye engine.
///
/// Obsolete (01/21/2026)
/// This file defines the `Renderable2DComponent` class, which represents
/// a renderable entity for 2D graphics. It extends the `IRenderableComponent`
/// base class and is responsible for drawing 2D objects such as sprites,
/// UI elements, and textures in the rendering system.
///

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"

namespace LaurelEye::Graphics {

    /// @class Renderable2DComponent
    /// @brief Represents a renderable 2D entity in the rendering system.
    ///
    /// `Renderable2DComponent` provides a lightweight interface for
    /// managing 2D drawable elements within the engine. It typically
    /// holds references to meshes and materials appropriate for 2D
    /// rendering such as sprites or UI textures.
    ///
    /// Future expansions may include animation data, sprite sheets,
    /// or specialized shader configurations for 2D rendering.
    class Renderable2DComponent : public IRenderableComponent {
    public:
        /// @brief Default constructor.
        ///
        /// Initializes the 2D renderable component with default parameters
        /// and prepares it for assignment of mesh and material data.
        Renderable2DComponent();
        /// @brief Constructs a 2D renderable component from a descriptor.
        /// @param rcd The renderable component descriptor containing mesh and material data.
        Renderable2DComponent(RenderableComponentDesc rcd);

    private:
        // Sprite, Texture, Etc
    };

} // namespace LaurelEye::Graphics
