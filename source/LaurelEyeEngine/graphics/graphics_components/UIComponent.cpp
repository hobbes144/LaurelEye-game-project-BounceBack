/// @file   UIComponent.cpp
/// @author Daoming Wang (KeitherJinn@Outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-30-2025
/// @brief Implementation of UI component on screen.

#include "LaurelEyeEngine/graphics/graphics_components/UIComponent.h"

namespace LaurelEye::Graphics {
    UIComponent::UIComponent()
        : Renderable2DComponent() {
        SetMesh(Graphics::Mesh::getShapeMesh(Graphics::Mesh::Square));
    }
    UIComponent::UIComponent(RenderableComponentDesc rcd)
        : Renderable2DComponent(rcd) {
        SetMesh(Graphics::Mesh::getShapeMesh(Graphics::Mesh::Square));
    }
} // namespace LaurelEye::Graphics
