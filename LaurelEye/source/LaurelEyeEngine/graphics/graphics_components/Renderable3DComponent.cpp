/// @file   Renderable3DComponent.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Renderable3D component

#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"

namespace LaurelEye::Graphics {
    Renderable3DComponent::Renderable3DComponent()
        : IRenderableComponent(RenderComponentType::Renderable3D, {}) {}
    Renderable3DComponent::Renderable3DComponent(RenderableComponentDesc rcd)
        : IRenderableComponent(RenderComponentType::Renderable3D, rcd) {}

} // namespace LaurelEye::Graphics
