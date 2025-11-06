/// @file   IWindowSurfaceProvider.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **digipen email**
///     anish.murthy@digipen.edu
/// @date   10-09-2025
/// @brief  Interface for window surface providers for different graphics backends.
///
/// This interface defines the functionality required for any graphics backend
/// to manage a window surface. This includes attaching to a window, managing
/// frame lifecycle, and querying backend-specific capabilities.

#pragma once

#include "LaurelEyeEngine/graphics/Graphics.h"
#include "LaurelEyeEngine/window/IWindow.h"

#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"

#include <cstdint>

namespace LaurelEye::Graphics {

    /// @struct SurfaceCapabilities
    /// @brief Describes the capabilities of a window surface or device.
    ///
    /// Currently implemented for OpenGL and stubbed for Vulkan. Provides
    /// information such as depth buffer bits, color buffer format, and
    /// other surface-related parameters.
    struct SurfaceCapabilities {
        uint32_t depthBits = 24;
        // Other capabilities to add later:
        // bool srgb (hdr or sRGB)
        // colorBits
        // stencilBits
        //
        // For Vulkan:
        // imageCount (1 for OpenGL, N=swapchain images for Vulkan)
    };

    using SurfaceHandle = uint32_t;

    namespace Surface {
        static constexpr SurfaceHandle InvalidSurface = UINT32_MAX;
    }

    inline bool isValidSurface(SurfaceHandle h) noexcept {
        return h != Surface::InvalidSurface;
    }

    /// @brief Implements Surface and related functionalities for the
    /// GraphicsBackend.
    ///
    /// Essentially everything in the GraphicsBackend that specifically
    /// relates to the Window is handled here.
    ///
    /// OpenGL: just the glViewport.
    /// Vulkan: Surface, Swapchain are owned by this.
    class IWindowSurfaceProvider {
    public:
        IWindowSurfaceProvider() {
            id = numSurfaces;
            ++numSurfaces;
        };
        /// @brief Virtual destructor.
        virtual ~IWindowSurfaceProvider() = default;

        /// @brief Get what GraphicsBackend is implemented
        /// @return The Graphics API Backend.
        virtual GraphicsBackend backend() const = 0;

        /// @brief Attaches the surface provider to a given window.
        /// @param window The window to attach the surface to.
        virtual void attachToWindow(IWindow&) = 0;
        /// @brief Shuts down the surface and releases any backend-specific resources.
        virtual void shutdown() = 0;

        /// @brief Prepares the surface for rendering a new frame.
        ///
        /// This function should be called at the beginning of each frame.
        virtual void beginFrame() = 0;
        /// @brief Finalizes rendering on the surface and presents it.
        ///
        /// This function should be called at the end of each frame.
        virtual void endFrame() = 0;

        virtual SizeRegistry getSize() { return windowSize; };
        virtual const SizeRegistry* getSizePointer() { return &windowSize; };

        virtual void resizeSurfaceCallback() = 0;

        virtual void setRenderSystemResizeCallback(
            std::function<void(SurfaceHandle, const SizeRegistry&)> _rsResizeCallback) = 0;

    protected:
        static inline uint32_t numSurfaces{0};

        SurfaceHandle id = Surface::InvalidSurface;

        /// @brief Handle to the native window associated with this surface.
        NativeWindowHandle nativeWindow;
        virtual void resizeSurface(int w, int h) = 0;

        // We would have SizeRegistry object here to manage viewport size.
        SizeRegistry windowSize;
    };
} // namespace LaurelEye::Graphics
