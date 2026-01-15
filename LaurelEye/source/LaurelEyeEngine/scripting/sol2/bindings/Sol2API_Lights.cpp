#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Lights.h"
#include "LaurelEyeEngine/graphics/resources/Lights.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/Material.h"
#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/PointLightComponent.h"

namespace LaurelEye::Scripting {
    void Sol2API_Lights::setup(sol::state& lua) {
        // Light resource structs
        lua.new_usertype<Graphics::DirectionalLight>(
            "DirectionalLight",
            sol::constructors<Graphics::DirectionalLight()>(),

            "direction", &Graphics::DirectionalLight::direction,
            "intensity", &Graphics::DirectionalLight::intensity,
            "color", &Graphics::DirectionalLight::color,
            "shadowIndex", &Graphics::DirectionalLight::shadowIndex);

        lua.new_usertype<Graphics::PointLight>(
            "PointLight",
            sol::constructors<Graphics::PointLight()>(),

            "position", &Graphics::PointLight::position,
            "intensity", &Graphics::PointLight::intensity,
            "color", &Graphics::PointLight::color,
            "range", &Graphics::PointLight::range,
            "shadowIndex", &Graphics::PointLight::shadowIndex);

        lua.new_usertype<Graphics::AmbientLight>(
            "AmbientLight",
            sol::constructors<Graphics::AmbientLight()>(),

            "color", &Graphics::AmbientLight::color,
            "intensity", &Graphics::AmbientLight::intensity);

        // Lights base
        lua.new_usertype<Graphics::LightComponent>(
            "LightComponent",

            sol::base_classes, sol::bases<Graphics::IRenderPropertyComponent>(),

            "getType", &Graphics::LightComponent::getType,
            "getInitStatus", &Graphics::LightComponent::getInitStatus
            // No handle or init setters to Lua unless you want them
        );

        // Ambient
        lua.new_usertype<Graphics::AmbientLightComponent>(
            "AmbientLightComponent",
            sol::constructors<Graphics::AmbientLightComponent(), Graphics::AmbientLightComponent(const Graphics::AmbientLight&)>(),
            "setLightData", &Graphics::AmbientLightComponent::setLightData,
            "getLightData", &Graphics::AmbientLightComponent::getLightData,
            sol::base_classes, sol::bases<Graphics::LightComponent>());

        // Directional
        lua.new_usertype<Graphics::DirectionalLightComponent>(
            "DirectionalLightComponent",
            sol::constructors<Graphics::DirectionalLightComponent(), Graphics::DirectionalLightComponent(const Graphics::DirectionalLight&)>(),
            "setLightData", &Graphics::DirectionalLightComponent::setLightData,
            "getLightData", &Graphics::DirectionalLightComponent::getLightData,
            sol::base_classes, sol::bases<Graphics::LightComponent>());

        // Point
        lua.new_usertype<Graphics::PointLightComponent>(
            "PointLightComponent",
            sol::constructors<Graphics::PointLightComponent(), Graphics::PointLightComponent(const Graphics::PointLight&)>(),
            "setLightData", &Graphics::PointLightComponent::setLightData,
            "getLightData", &Graphics::PointLightComponent::getLightData,
            sol::base_classes, sol::bases<Graphics::LightComponent>());

    }
} // namespace LaurelEye::Scripting
