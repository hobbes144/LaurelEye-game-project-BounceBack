/// @file   Lights.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    04-04-2025
/// @brief Lights supported by the engine

#pragma once

#include "LaurelEyeEngine/graphics/resources/Shadow.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include <cstdint>

namespace LaurelEye::Graphics {
    enum class LightType {
        Default,
        Directional,
        Point,
        Ambient
    };

    /// @class DirectionalLight
    /// @brief Directional lights such as the sun.
    ///
    /// These are lights that do not have a point position, but rather apply as
    /// constant direction light rays, similar to the sun. This can also be
    /// used for lasers, spotlights and other highly focused lights.
    ///
    /// NOTE: Flashlights and are not covered under this version of
    /// DirectionalLight. They have some dispersion so they would have an angle
    /// of spread and a focus point. I'd likely implement them as a separate
    /// type of light.
    ///
    struct DirectionalLight {
        Vector3 direction;
        float intensity;
        Vector3 color;
        uint32_t shadowIndex = Shadow::NoShadow;
    };

    /// @class PointLight
    /// @brief Point lights such as lightbulbs and glowing orbs.
    ///
    /// These are lights that have a definite position in the scene. They are
    /// typically lamps and bulbs. These are typically implemented with a range
    /// and intensity. The scale of intensity dropoff is subjective to the
    /// shader's implementation.
    ///
    /// This is the type of light you would use if you wanted to have lights in
    /// a scene apart from the ambient light and the sun.
    ///
    struct PointLight {
        Vector3 position;
        float intensity;
        Vector3 color;
        float range;
        uint32_t shadowIndex = Shadow::NoShadow;
        int32_t active = (int32_t)false;
        // Struct size is 36, so we need to pad after to align next struct to 16 byte
        // boundary.
        int32_t padding[2];
    };

    /// @class AmbientLight
    /// @brief Ambient lights to light normally unlit parts of the scene.
    ///
    /// Ambient lights are applied uniformly to all the objects in the scene.
    /// They ensure that no parts of the scene are black because of lack of
    /// light and are needed for good looking scenes in non-ray traced renders.
    ///
    /// Ambient lights are also applied to lit objects, and work as a rough
    /// global brightness.
    ///
    struct AmbientLight {
        Vector3 color;
        float intensity;
    };

    /// @class GlobalLights
    /// @brief Lights that are global to a scene.
    ///
    /// These are applied to all objects in the scene and have no range. This
    /// includes the sun and ambient light.
    ///
    struct GlobalLights {
        DirectionalLight sunLight;
        AmbientLight ambientLight;
    };

    /// @class LocalLights
    /// @brief Lights that affect a localized region of a scene.
    ///
    /// These are typically lights like bulbs, lamps, etc.
    ///
    /// TODO: Remove directional and ambient lights and add localized variants
    /// of them. Also add any future light types here.
    ///
    struct LocalLights {
        std::vector<PointLight> pointLights;
    };

    /// @class LocalLightsStatus
    /// @brief Stores if a global light's buffered data should be updated due to a
    /// change in the light.
    ///
    /// Directly maps to the corresponding GlobalLights struct.
    ///
    struct GlobalLightsStatus {
        bool sunDirty = false;
        bool ambientDirty = false;
    };
    struct LocalLightsStatus {
        std::vector<unsigned int> pointDirty = {};
    };
} // namespace LaurelEye::Graphics
