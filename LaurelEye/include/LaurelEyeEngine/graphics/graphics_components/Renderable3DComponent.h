/// @file   Renderable3DComponent.h
/// @author Nick Shaw
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10-07-2025
/// @brief  3D renderable component for the LaurelEye engine.
///
/// This file defines the `Renderable3DComponent` class, which represents
/// a renderable entity for 3D graphics. It extends the `IRenderableComponent`
/// interface and serves as a base for 3D objects rendered in the scene,
/// such as models, meshes, and static or dynamic geometry.
///

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"

namespace LaurelEye::Graphics {

    /// @class Renderable3DComponent
    /// @brief Represents a renderable 3D entity in the rendering system.
    ///
    /// `Renderable3DComponent` encapsulates mesh and material data required
    /// to render 3D objects within the engine. It is responsible for maintaining
    /// renderable geometry and appearance information, and may later be extended
    /// to include level-of-detail (LOD) management, lighting data, or skeletal
    /// animation support.
    class Renderable3DComponent : public IRenderableComponent {

    public:
        /// @brief Default constructor.
        ///
        /// Initializes the 3D renderable component with default parameters.
        /// Mesh and material references can be assigned later through
        /// `SetMesh` and `SetMaterial`.
        Renderable3DComponent();
        /// @brief Constructs a 3D renderable component from a descriptor.
        /// @param rcd The renderable component descriptor containing mesh and material data.
        Renderable3DComponent(RenderableComponentDesc rcd);

    private:

    };

} // namespace LaurelEye::Graphics
