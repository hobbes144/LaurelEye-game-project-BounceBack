/// @file   Renderable2DComponent.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Renderable2D component

#include "LaurelEyeEngine/graphics/graphics_components/Renderable2DComponent.h"

namespace LaurelEye::Graphics {
    Renderable2DComponent::Renderable2DComponent()
        : IRenderableComponent(RenderComponentType::Renderable2D, {}) {}
    Renderable2DComponent::Renderable2DComponent(RenderableComponentDesc rcd)
        : IRenderableComponent(RenderComponentType::Renderable2D, rcd) {}


} // namespace LaurelEye::Graphics
