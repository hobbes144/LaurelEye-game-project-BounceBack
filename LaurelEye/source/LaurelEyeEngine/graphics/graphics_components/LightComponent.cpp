#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"

namespace LaurelEye::Graphics {

    LightComponent::LightComponent()
        : IRenderPropertyComponent(RenderComponentType::PropertyLight),
          type(LightType::Default) {

    }

} // namespace LaurelEye::Graphics
