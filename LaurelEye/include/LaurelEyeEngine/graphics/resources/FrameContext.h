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
    class MeshManager;
    class IRenderComponent;
    class ShadowManager;

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
        ///
        /// NOTE: This should be switched to using RenderRegistry so we only
        /// send in the specific GPU info and can use a single FrameContext
        /// for all render passes, with multiple vectors specific to each
        /// RenderPass' requirements. Eg. We have particles, skinned meshes,
        /// static meshes, ui elements, etc.
        std::vector<IRenderComponent*>& objects;
        // shadows
        ShadowManager* shadowManager;
    };
} // namespace LaurelEye::Graphics
