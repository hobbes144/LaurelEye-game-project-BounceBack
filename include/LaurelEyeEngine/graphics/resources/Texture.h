#pragma once

#include <cstdint>
#include <string>

namespace LaurelEye::Graphics {

    /// @enum TextureFormat
    /// @brief Supported GPU texture formats.
    ///
    /// These formats define how pixel data is stored in memory.
    enum class TextureFormat {
        RGBA8,
        RGBA16F,
        RGBA32F,
        RGB8,
        RGB16F,
        RGB32F,
        R8,
        R16F,
        R32F,
        DEPTH32F,
        DEPTH24,
    };

    /// @enum TextureFilterMode
    /// @brief Describes how textures are sampled when magnified or minified.
    enum class TextureFilterMode {
        Nearest,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear
    };

    /// @enum TextureWrapMode
    /// @brief Describes how textures behave when UV coordinates go out of bounds.
    enum class TextureWrapMode {
        Repeat,
        MirroredRepeat,
        ClampToEdge
    };

    /// @enum TextureType
    /// @brief Supported types of texture dimensionality.
    enum class TextureType {
        Texture2D,
        Texture3D,
        TextureStorage2D
    };

    /// @enum TextureMipMode
    /// @brief Describes how mipmaps are managed.
    enum class TextureMipMode {
        None,
        AutoGenerate,
        Manual
    };

    enum class SampleCount {
        X1,
        X2,
        X4,
        X8
    };

    inline int getSampleIntFromSampleCount(const SampleCount& sampleCount) noexcept {
        switch ( sampleCount ) {
        case SampleCount::X8:
            return 8;
        case SampleCount::X4:
            return 4;
        case SampleCount::X2:
            return 2;
        case SampleCount::X1:
        default:
            return 1;
        }
    }

    // @typedef TextureHandle
    /// @brief Numeric handle used to reference a texture.
    using TextureHandle = uint32_t;
    static constexpr TextureHandle InvalidTexture = UINT32_MAX;
    inline bool isValidTexture(TextureHandle h) noexcept {
        return h != InvalidTexture;
    }

    /// @struct TextureDesc
    /// @brief Describes the properties required to create a texture.
    ///
    /// Used by the render device or resource manager to allocate GPU memory
    /// and define how the texture will be used in rendering.
    struct TextureDesc {
        std::string debugName;                       ///< Optional debug name for identification.
        TextureType type = TextureType::Texture2D;   ///< Texture dimensionality.
        TextureFormat format = TextureFormat::RGBA8; ///< Pixel format.
        uint32_t width = 0;                          ///< Width of the texture in pixels.
        uint32_t height;                             ///< Height of the texture in pixels.
        // uint32_t depth = 1; ///< Depth for 3D textures (optional).
        TextureMipMode mipMode = TextureMipMode::AutoGenerate; ///< Mipmap generation mode.
        uint32_t mipLevels = 1;                                ///< Number of mip levels. Overwritten if AutoGenerate is used.
        // WARNING: Samples are only supported for Texture2D right now.
        SampleCount samples = SampleCount::X1;
    };

    /// @struct SamplerDesc
    /// @brief Defines how a texture is sampled when accessed by shaders.
    ///
    /// Includes filtering and wrapping behavior for texture coordinates.
    struct SamplerDesc {
        TextureFilterMode magFilter = TextureFilterMode::Nearest;             ///< Magnification filter mode.
        TextureFilterMode minFilter = TextureFilterMode::NearestMipmapLinear; ///< Minification filter mode.
        TextureWrapMode sWrap = TextureWrapMode::ClampToEdge;                 ///< Wrapping mode for U (S) coordinate.
        TextureWrapMode tWrap = TextureWrapMode::ClampToEdge;                 ///< Wrapping mode for V (T) coordinat
    };

} // namespace LaurelEye::Graphics
