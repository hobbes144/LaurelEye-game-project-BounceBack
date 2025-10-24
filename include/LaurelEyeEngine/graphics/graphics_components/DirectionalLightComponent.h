/// @file   DirectionalLightComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief Derived light component class for directional light.

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"

namespace LaurelEye::Graphics {

    /// @class DirectionalLightComponent
    /// @brief Component to register directional lights.
    class DirectionalLightComponent : public LightComponent {

    public:
        DirectionalLightComponent();
        DirectionalLightComponent(const DirectionalLight& data);

        /// @brief Set light data.
        ///
        /// @param data DirectionalLight data.
        void setLightData(const DirectionalLight& data) { lightData = data; }
        /// @brief Get light data.
        ///
        /// @return DirectionalLight data.
        DirectionalLight& getLightData() { return lightData; }

    protected:
        DirectionalLight lightData;
    };

} // namespace LaurelEye::Graphics
