/// @file   AmbientLightComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief Derived light component class for ambient light.

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"

namespace LaurelEye::Graphics {

    /// @class AmbientLightComponent
    /// @brief Component to register ambient lights.
    class AmbientLightComponent : public LightComponent {

    public:
        AmbientLightComponent();
        AmbientLightComponent(const AmbientLight& data);

        /// @brief Set light data.
        ///
        /// @param data AmbientLight data.
        void setLightData(const AmbientLight& data) { lightData = data; }
        /// @brief Get light data.
        ///
        /// @return AmbientLight data.
        AmbientLight& getLightData() { return lightData; }

    protected:
        AmbientLight lightData;
    };

} // namespace LaurelEye::Graphics
