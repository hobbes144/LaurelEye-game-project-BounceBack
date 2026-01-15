#include "LaurelEyeEngine/graphics/graphics_components/IRenderComponent.h"

#include "LaurelEyeEngine/graphics/RenderSystem.h"

namespace LaurelEye::Graphics {
    IRenderComponent::~IRenderComponent() {
        //rs->deregisterComponent(this);
    }
} // namespace LaurelEye::Graphics
