/// @file   UIComponent.cpp
/// @author Daoming Wang (KeitherJinn@Outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-30-2025
/// @brief Implementation of UI component on screen.

#include "LaurelEyeEngine/graphics/graphics_components/UIComponent.h"
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"

namespace LaurelEye::Graphics {
    UIComponent::UIComponent()
        : Renderable2DComponent() {
        SetMeshPrimitiveType(PrimitiveMeshType::Square);
        uiName = "DefaultUIComponent";
    }

    UIComponent::UIComponent(RenderableComponentDesc rcd)
        : Renderable2DComponent(rcd) {
        SetMeshPrimitiveType(PrimitiveMeshType::Square);
        uiName = "DefaultUIComponent";
    }

    void UIComponent::SetIsFocused(bool newIsFocused) {
        isFocused = newIsFocused;
        UpdateScaling();
    }

    void UIComponent::UpdateScaling() {
        Vector3 scale = Vector3(originalScale.x / window->getWidth(), originalScale.y / window->getHeight(), 0.0f);
        if ( isFocused ) {
            transComp->getWorldTransform().setScaling(scale * 1.1f);
        }
        else {
            transComp->getWorldTransform().setScaling(scale);
        }
    }

    void UIComponent::BindTransform() {
        Renderable2DComponent::BindTransform();
        if ( isFocused ) {
            SetOriginalScale(transComp->getWorldScale() * 10 / 11);
        }
        else {
            SetOriginalScale(transComp->getWorldScale());
        }
    }

    bool UIComponent::MouseInRange(float mouseX, float mouseY) {
        float halfWidth = transComp->getWorldScale().x;
        float halfHeight = transComp->getWorldScale().y;
        float xMin = transComp->getWorldPosition().x - halfWidth;
        float xMax = transComp->getWorldPosition().x + halfWidth;
        float yMin = transComp->getWorldPosition().y - halfHeight;
        float yMax = transComp->getWorldPosition().y + halfHeight;
        return (mouseX >= xMin && mouseX <= xMax && mouseY >= yMin && mouseY <= yMax);
    }
} // namespace LaurelEye::Graphics
