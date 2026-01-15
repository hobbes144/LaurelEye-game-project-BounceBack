/// @file   RenderRegistry.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Render registry that helps send only the required objects to render
/// passes.
/// Currently a stub, leaving this for reference when we implement.
#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/IRenderComponent.h"

namespace LaurelEye::Graphics {

    /// @class RenderRegistry
    /// @brief Render registry allows us to send a processed list of objects
    /// to render calls.
    class RenderRegistry {
    public:
        std::vector<IRenderComponent>* registry;
    };
}
