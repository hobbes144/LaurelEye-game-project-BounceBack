#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Camera.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"

namespace LaurelEye::Scripting {
    void Sol2API_Camera::setup(sol::state& lua) {
        lua.new_usertype<Graphics::CameraComponent>(
            "CameraComponent",

            sol::constructors<Graphics::CameraComponent()>(),

            // ---- Identifiers ----
            "getName", &Graphics::CameraComponent::getName,
            "setName", &Graphics::CameraComponent::setName,

            // ---- View / Projection ----
            "updateViewMatrix", &Graphics::CameraComponent::updateViewMatrix,
            "getViewMatrix", &Graphics::CameraComponent::getViewMatrix,
            "getInverseViewMatrix", &Graphics::CameraComponent::getInverseViewMatrix,
            "getProjectionMatrix", &Graphics::CameraComponent::getProjectionMatrix,

            // ---- Projection API ----
            "setPerspectiveProjection", &Graphics::CameraComponent::setPerspectiveProjection,
            "setOrthographicProjection", &Graphics::CameraComponent::setOrthographicProjection,
            "updateAspectRatio", &Graphics::CameraComponent::updateAspectRatio,

            // ---- Exposure ----
            "getExposure", &Graphics::CameraComponent::getExposure,
            "setExposure", &Graphics::CameraComponent::setExposure,

            // ---- Inheritance ----
            sol::base_classes, sol::bases<Graphics::IRenderPropertyComponent>());
    }
} // namespace LaurelEye::Scripting
