#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"

namespace LaurelEye::Graphics {

    DirectionalLightComponent::DirectionalLightComponent()
        : LightComponent() {
        type = LightType::Directional;
        lightData = DirectionalLight{
            Vector3(-0.2f, -1.0f, -0.3f), // direction
            1.0f,                        // intensity
            Vector3(1.0f, 1.0f, 1.0f)    // color (white)
        };
    }

    DirectionalLightComponent::DirectionalLightComponent(const DirectionalLight& data)
        : LightComponent(), lightData(data) {
        type = LightType::Directional;
    }

} // namespace LaurelEye::Graphics
