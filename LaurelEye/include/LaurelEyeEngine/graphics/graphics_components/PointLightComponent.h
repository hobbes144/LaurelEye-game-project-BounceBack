/// @file   AmbientLightComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief Derived light component class for point light.

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"

namespace LaurelEye::Graphics {

    /// @class PointLightComponent
    /// @brief Component to register point lights.
    class PointLightComponent : public LightComponent {

    public:
        PointLightComponent();
        PointLightComponent(const PointLight& data);

        /// @brief Set light data.
        ///
        /// @param data PointLight data.
        void setLightData(const PointLight& data) { lightData = data; }
        /// @brief Get light data.
        ///
        /// @return DirectionalLight data.
        PointLight& getLightData() { return lightData; }

    protected:
        PointLight lightData;
    };

} // namespace LaurelEye::Graphics
