/// @file   FrameContext.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Frame Contect class that stores the data to be passed to the Render Passes.
#pragma once

#include <vector>

namespace LaurelEye::Graphics {

    class IRenderDevice;
    class RenderResources;
    class IRenderComponent;

    /// @struct FrameContext
    /// @brief Framecontext stores the data to be passed to the Render Passes.
    struct FrameContext {
        /// @brief delta time in seconds
        float deltaSeconds;
        /// @brief The being used to render the objects to the screen
        IRenderDevice& device;
        /// @brief The Render Resources
        RenderResources& resources;
        /// @brief The IRenderComponents to be drawn to the screen
        std::vector<IRenderComponent*>& objects;
    };
} // namespace LaurelEye::Graphics
