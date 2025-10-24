#include "LaurelEyeEngine/graphics/graphics_components/PointLightComponent.h"

namespace LaurelEye::Graphics {

    PointLightComponent::PointLightComponent()
        : LightComponent() {
        type = LightType::Point;
        lightData = PointLight{
            Vector3(0.0f, 0.0f, 0.0f), // position
            1.0f,                     // intensity
            Vector3(1.0f, 1.0f, 1.0f), // color (white)
            10.0f                     // range
        };
    }

    PointLightComponent::PointLightComponent(const PointLight& data)
        : LightComponent(), lightData(data) {
        type = LightType::Point;
    }

} // namespace LaurelEye::Graphics
