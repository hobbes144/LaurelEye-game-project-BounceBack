/// @file   UIComponent.h
/// @author Daoming Wang
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   10-30-2025
/// @brief  UI component for the LaurelEye engine.

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/Renderable2DComponent.h"

namespace LaurelEye::Graphics {

    /// @class UIComponent
    /// @brief Represents a renderable 2D UI entity in the rendering system.
    /// 
    class UIComponent : public Renderable2DComponent {
    public:
        /// @brief Default constructor.
        ///
        /// Initializes the component with default parameters
        /// and prepares it for assignment of mesh and material data.
        UIComponent();
        /// @brief Constructs a UI component from a descriptor.
        /// @param rcd The renderable component descriptor containing mesh and material data.
        UIComponent(RenderableComponentDesc rcd);

    private:
        //Sprite, Texture, Etc

    };

}
