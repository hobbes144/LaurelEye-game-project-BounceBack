/// @file   LightComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief A light component base class for different kinds of lights.

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/IRenderPropertyComponent.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Lights.h"
#include <cstdint>

namespace LaurelEye::Graphics {

    /// @class LightComponent
    /// @brief Component to register lights.
    ///
    class LightComponent : public IRenderPropertyComponent {

    public:
        LightComponent();

        /// @brief Get the type of the light
        ///
        /// @return LightType.
        LightType getType() const { return type; }

        // TODO: Move GPU specific calls to private and make RenderSystem a friend.


        /// @brief Set light DataBuffer.
        ///
        /// @param handle Light DataBufferHandle.
        virtual void setHandle(DataBufferHandle handle) { lightBufferHandle = handle; }
        /// @brief Get light DataBuffer.
        ///
        /// @return Light DataBufferHandle.
        virtual DataBufferHandle getHandle() const { return lightBufferHandle; }

        /// @brief Set if the light buffer was initialized.
        ///
        /// This is meant to store if the buffer for this light was created.
        /// Typically we do not want to keep updating the light buffers for
        /// such little data. It is better to store the lights in buffers and
        /// the binding points during draw calls so that there's no pointless
        /// CPU->GPU data transfers (which are very slow).
        ///
        /// This is different per light type, we typically store the
        /// sun (DirectionalLight) and AmbientLight as values in a shared global
        /// lights data buffer, while the remaining lights are stored in the
        /// local lights data buffer.
        ///
        /// @param status Initialization status.
        void setInitStatus(bool status) { isInit = status; }
        /// @brief Get if the light buffer was initialized.
        ///
        /// @return initialization status.
        bool getInitStatus() const { return isInit; }

    protected:
        LightType type;
        DataBufferHandle lightBufferHandle;
        bool isInit = false;
    };

} // namespace LaurelEye::Graphics

