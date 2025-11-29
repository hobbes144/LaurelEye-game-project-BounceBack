/// @file   UIComponent.h
/// @author Daoming Wang
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   10-30-2025
/// @brief  UI component for the LaurelEye engine.

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/Renderable2DComponent.h"
#include "LaurelEyeEngine/input/InputManager.h"
#include "LaurelEyeEngine/window/IWindow.h"

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

        virtual void SetIsFocused(bool newIsFocused);
        void UpdateScaling();

        void SetOriginalScale(const Vector3& scale) { originalScale = scale; }

        void SetCanBeFocused(bool canFocus) { canBeFocused = canFocus; }
        bool CanBeFocused() const { return canBeFocused; }
        void SetUIName(std::string newTag) { uiName = newTag; }
        std::string GetUIName() const { return uiName; }

        void SetCanBeActivated(bool canActivate) { canBeActivated = canActivate; }
        bool CanBeActivated() const { return canBeActivated; }

        virtual void BindTransform() override;
        void BindWindow(LaurelEye::IWindow* _window) { window = _window; }

        bool MouseInRange(float mouseX, float mouseY);

    protected:
        std::string uiName;
        bool canBeFocused = false;
        bool isFocused = false;
        bool canBeActivated = false;

        Vector3 originalScale = Vector3();
        LaurelEye::IWindow* window = nullptr;
    };
} // namespace LaurelEye::Graphics
