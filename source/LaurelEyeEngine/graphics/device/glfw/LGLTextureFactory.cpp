/// @file   LGLTextureFactory.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Texture Factory for OpenGL

#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"

#include "LaurelEyeEngine/graphics/resources/Texture.h"

namespace LaurelEye::Graphics {

    GLint textureFormatToGLFormat(TextureFormat f) {
        switch ( f ) {
        case TextureFormat::RGBA8:
            return GL_RGBA8;
        case TextureFormat::RGBA16F:
            return GL_RGBA16F;
        case TextureFormat::RGBA32F:
            return GL_RGBA32F;
        case TextureFormat::RGB8:
            return GL_RGB8;
        case TextureFormat::RGB16F:
            return GL_RGB16F;
        case TextureFormat::RGB32F:
            return GL_RGB32F;
        case TextureFormat::R8:
            return GL_R8;
        case TextureFormat::R16F:
            return GL_R16F;
        case TextureFormat::R32F:
            return GL_R32F;
        }

        assert(false && "ERROR::RENDERSYSTEM::INVALIDFORMAT");
    }

    GLint textureFormatToGLBaseFormat(TextureFormat f) {
        std::vector<TextureFormat> rgbFormats = {TextureFormat::RGB8, TextureFormat::RGB16F, TextureFormat::RGB32F};
        std::vector<TextureFormat> rgbaFormats = {TextureFormat::RGBA8, TextureFormat::RGBA16F, TextureFormat::RGBA32F};
        GLint baseFormat;

        if ( std::any_of(rgbFormats.begin(), rgbFormats.end(), [&](TextureFormat e) { return e == f; }) ) {
            baseFormat = GL_RGB;
        }
        else if ( std::any_of(rgbFormats.begin(), rgbFormats.end(), [&](TextureFormat e) { return e == f; }) ) {
            baseFormat = GL_RGBA;
        }
        else {
            // TODO: Add code to handle 1D texture R8 here
            std::cout << "WARNING::RENDERSYSTEM::TEXTUREFACTORY::CREATE::INVALIDFORMAT"
                      << "::Defaulting to RGBA" << std::endl;
            baseFormat = GL_RGBA;
        }

        return baseFormat;
    }

    GLint textureWrapToGLWrap(TextureWrapMode w) {
        switch ( w ) {
        case TextureWrapMode::Repeat:
            return GL_REPEAT;
        case TextureWrapMode::MirroredRepeat:
            return GL_MIRRORED_REPEAT;
        case TextureWrapMode::ClampToEdge:
            return GL_CLAMP_TO_EDGE;
        }

        assert(false && "ERROR::RENDERSYSTEM::INVALIDWRAPMODE");
    }

    GLint textureFilterToGLFilter(TextureFilterMode f) {
        switch ( f ) {
        case TextureFilterMode::Nearest:
            return GL_NEAREST;
        case TextureFilterMode::Linear:
            return GL_LINEAR;
        case TextureFilterMode::NearestMipmapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilterMode::NearestMipmapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilterMode::LinearMipmapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilterMode::LinearMipmapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;
        }

        assert(false && "ERROR::RENDERSYSTEM::INVALIDFILTERMODE");
    }

    bool textureFilterNeedsMip(TextureFilterMode f) {
        switch ( f ) {
        case TextureFilterMode::NearestMipmapNearest:
        case TextureFilterMode::NearestMipmapLinear:
        case TextureFilterMode::LinearMipmapNearest:
        case TextureFilterMode::LinearMipmapLinear:
            return true;
        default:
            return false;
        }
    }

    TextureHandle LGLTextureFactory::create(const TextureDesc& d, const void* init) {
        TextureHandle h;
        glGenTextures(1, &h);

        textures[h] = d;

        assert(d.type == TextureType::Texture2D && "ERROR::RENDERSYSTEM::TEXTUREFACTORY::CREATE::INVALIDTYPE::Texture3D not supported");
        glBindTexture(GL_TEXTURE_2D, h);

        glTexImage2D(GL_TEXTURE_2D, 0, textureFormatToGLFormat(d.format),
                     d.width, d.height, 0, textureFormatToGLBaseFormat(d.format), GL_UNSIGNED_BYTE, init);

        if ( init != nullptr && d.mipMode == TextureMipMode::AutoGenerate )
            glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        return h;
    }

    void LGLTextureFactory::generateMips(TextureHandle h) {
        glBindTexture(GL_TEXTURE_2D, h);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LGLTextureFactory::resize(TextureHandle h, uint32_t newW, uint32_t newH, uint32_t newD) {
        assert((textures.find(h) != textures.end()) && "ERROR::RENDERSYSTEM::TEXTUREFACTORY::RESIZE::INVALID_TEXTURE_HANDLE");
        TextureDesc& d = textures[h];
        d.width = newW;
        d.height = newH;

        glBindTexture(GL_TEXTURE_2D, h);

        glTexImage2D(
            GL_TEXTURE_2D, 0, textureFormatToGLFormat(d.format),
            newW, newH, 0, textureFormatToGLBaseFormat(d.format), GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LGLTextureFactory::setSampler(TextureHandle h, const SamplerDesc& sDesc) {
        glBindTexture(GL_TEXTURE_2D, h);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, sDesc.maxLevel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilterToGLFilter(sDesc.magFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilterToGLFilter(sDesc.minFilter));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapToGLWrap(sDesc.sWrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapToGLWrap(sDesc.tWrap));

        if ( textureFilterNeedsMip(sDesc.minFilter) || textureFilterNeedsMip(sDesc.magFilter) ) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LGLTextureFactory::destroy(TextureHandle h) {
        auto it = textures.find(h);
        if ( it == textures.end() ) return;
        glDeleteTextures(1, &h);
        textures.erase(it);
    }

    void LGLTextureFactory::destroyAll() {
        for ( auto& [h, d] : textures ) {
            glDeleteTextures(1, &h);
        }
        textures.clear();
    }

} // namespace LaurelEye::Graphics
