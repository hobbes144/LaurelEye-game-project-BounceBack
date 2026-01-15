#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"

namespace LaurelEye::Graphics {

    AmbientLightComponent::AmbientLightComponent()
        : LightComponent() {
        type = LightType::Ambient;
    }

    AmbientLightComponent::AmbientLightComponent(const AmbientLight& data)
        : LightComponent(), lightData(data) {
        type = LightType::Ambient;
    }

} // namespace LaurelEye::Graphics
