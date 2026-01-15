/// @file   UIButtonComponent.h
/// @author Daoming Wang
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   10-30-2025
/// @brief  UI button component for the LaurelEye engine.

#pragma once
#include "LaurelEyeEngine/events/EventManager.h"
#include "LaurelEyeEngine/graphics/graphics_components/UIComponent.h"

namespace LaurelEye::Graphics {
    class UIButtonComponent : public UIComponent {
    public:
        UIButtonComponent();
        UIButtonComponent(RenderableComponentDesc rcd);

    private:
        // Calls a "UIButtonPressed" event
        void Activate();

        std::shared_ptr<LaurelEye::EventManager> eventManager;
    };
} // namespace LaurelEye::Graphics
