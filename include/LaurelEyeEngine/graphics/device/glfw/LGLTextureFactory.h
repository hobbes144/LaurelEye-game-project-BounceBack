/// @file   LGLTextureFactory.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-08-2025
/// @brief Texture Factory for OpenGL

#pragma once

#include "LaurelEyeEngine/graphics/resources/Texture.h"

#include <glad/glad.h>

#include <unordered_map>

namespace LaurelEye::Graphics {

    /// @brief Convert TextureFormat to OpenGL Texture Format.
    ///
    /// @param f TextureFormat enum value.
    /// @return OpenGL Texture Format.
    GLint textureFormatToGLFormat(TextureFormat f);

    /// @brief Convert TextureFormat to OpenGL Base Texture Format.
    ///
    /// This is used to create the appropriate texture storage channels.
    ///
    /// @param f TextureFormat enum value.
    /// @return OpenGL Texture Format.
    GLint textureFormatToGLBaseFormat(TextureFormat f);

    /// @brief Convert TextureWrapMode to OpenGL Texture Wrap Mode.
    ///
    /// @param w TextureWrapMode enum value.
    /// @return OpenGL Texture Wrap Mode.
    GLint textureWrapToGLWrap(TextureWrapMode w);

    /// @brief Convert TextureFilterMode to OpenGL Texture Filter Mode.
    ///
    /// @param f TextureFilterMode enum value.
    /// @return OpenGL Texture Filter Mode.
    GLint textureFilterToGLFilter(TextureFilterMode f);

    GLint textureFormatToGLUploadFormat(TextureFormat f);

    /// @brief Does the requested TextureFilterMode require Mipmaps
    ///
    /// @param f TextureFilterMode enum value.
    /// @return `true` if Mipmaps needed.
    bool textureFilterNeedsMip(TextureFilterMode f);

    bool textureIsTextureStorage(TextureType t);

    class LGLTextureFactory {
    public:
        ~LGLTextureFactory();

        /// \copydoc IRenderDevice::createTexture
        [[nodiscard]] TextureHandle create(const TextureDesc& d, const void* init = nullptr);

        TextureHandle createTextureStorage(const TextureDesc& d);

        TextureHandle createTexture(const TextureDesc& d, const void* init = nullptr);

        /// \copydoc IRenderDevice::generateTextureMips
        void generateMips(TextureHandle h);

        /// \copydoc IRenderDevice::resizeTexture
        void resize(TextureHandle h, uint32_t newW, uint32_t newH, uint32_t newD = 1);

        /// \copydoc IRenderDevice::setTextureSampler
        void setSampler(TextureHandle h, const SamplerDesc& sDesc);

        /// \copydoc IRenderDevice::destroyTexture
        void destroy(TextureHandle h);

        /// \copydoc IRenderDevice::destroyAllTextures
        void destroyAll();

        void bind(TextureHandle h, uint32_t textureUnit);

        /* Samplers */
        // New OpenGL prefers separated Texture and Sampler.
        // We can implement this if we move to that.
        // SamplerHandle createSampler(const SamplerDesc& sd);
        // void destroySampler(SamplerHandle s);
    private:
        std::unordered_map<TextureHandle, TextureDesc> textures;
    };
} // namespace LaurelEye::Graphics
