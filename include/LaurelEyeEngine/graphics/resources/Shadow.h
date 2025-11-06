#pragma once

#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"

#include <cstdint>

namespace LaurelEye::Graphics {

    enum class ShadowType {
        Simple,
        Volumetric
    };

    enum class ShadowSourceType {
        Directional,
        Point
    };

    // @typedef TextureHandle
    /// @brief Numeric handle used to reference a texture.
    using ShadowHandle = uint32_t;

    namespace Shadow {
        static constexpr ShadowHandle NoShadow = UINT32_MAX - 1;
        static constexpr ShadowHandle ShadowPending = UINT32_MAX;

    } // namespace Shadow
    inline bool isValidShadow(ShadowHandle h) noexcept {
        return h < Shadow::NoShadow;
    }
    inline bool isShadowPending(ShadowHandle h) noexcept {
        return h == Shadow::ShadowPending;
    }
    inline bool isNoShadow(ShadowHandle h) noexcept {
        return h == Shadow::NoShadow;
    }

    /// @struct TextureDesc
    /// @brief Describes the properties required to create a texture.
    ///
    /// Used by the render device or resource manager to allocate GPU memory
    /// and define how the texture will be used in rendering.
    struct ShadowDesc {
        SizeRegistry resolution = SizeRegistry{1024, 1024};
        ShadowType type = ShadowType::Simple;
    };

} // namespace LaurelEye::Graphics
