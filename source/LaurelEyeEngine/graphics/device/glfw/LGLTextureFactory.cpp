/// @file   LGLTextureFactory.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Texture Factory for OpenGL

#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"
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
        case TextureFormat::DEPTH32F:
            return GL_DEPTH_COMPONENT32F;
        }

        assert(false && "ERROR::RENDERSYSTEM::INVALIDFORMAT");
        return GL_RGBA8;
    }

    GLint textureFormatToGLBaseFormat(TextureFormat f) {
        switch ( f ) {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:
            return GL_RED;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
            return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
            return GL_RGBA;
        default:
            // TODO: Replace this with proper warning logging.
#if !defined(NDEBUG)
            std::cerr << "WARNING::RENDERSYSTEM::TEXTUREFACTORY::CREATE::INVALIDFORMAT"
                      << "::Defaulting to RGBA" << std::endl;
#endif
            return GL_RGBA;
        }
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
        return GL_REPEAT;
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
        return GL_LINEAR;
    }

    GLint textureFormatToGLUploadFormat(TextureFormat f) {
        switch ( f ) {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
            return GL_FLOAT;
        default:
            // TODO: Replace this with proper warning logging.
#if !defined(NDEBUG)
            std::cerr << "WARNING::RENDERSYSTEM::TEXTUREFACTORY::CREATE::INVALIDFORMAT"
                      << "::Defaulting to RGBA" << std::endl;
#endif
            return GL_FLOAT;
        }
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

    bool textureIsTextureStorage(TextureType t) {
        switch ( t ) {
        case TextureType::TextureStorage2D:
            return true;
        default:
            return false;
        }
    }

    LGLTextureFactory::~LGLTextureFactory() {
        destroyAll();
    }

    TextureHandle LGLTextureFactory::create(const TextureDesc& d, const void* init) {
        if ( textureIsTextureStorage(d.type) ) {
            return createTextureStorage(d);
        }
        else {
            return createTexture(d, init);
        }
    }

    TextureHandle LGLTextureFactory::createTextureStorage(const TextureDesc& d) {
        TextureHandle h;

        assert(
            d.type == TextureType::TextureStorage2D &&
            "ERROR::RENDERSYSTEM::TEXTUREFACTORY::CREATE::INVALIDTYPE::TextureStorage3D not supported");

        glCreateTextures(GL_TEXTURE_2D, 1, &h);

        textures[h] = d;

        glTextureStorage2D(h, d.mipLevels,
                           textureFormatToGLFormat(d.format),
                           d.width, d.height);

        // Reasonable defaults (you’ll likely use separate samplers anyway)
        glTextureParameteri(h, GL_TEXTURE_MIN_FILTER, d.mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTextureParameteri(h, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(h, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(h, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        return h;
    }

    TextureHandle LGLTextureFactory::createTexture(const TextureDesc& d, const void* init) {
        TextureHandle h;

        assert(
            d.type == TextureType::Texture2D &&
            "ERROR::RENDERSYSTEM::TEXTUREFACTORY::CREATE::INVALIDTYPE::Texture3D not supported");

        glGenTextures(1, &h);

        textures[h] = d;

        glBindTexture(GL_TEXTURE_2D, h);

        glTexImage2D(
            GL_TEXTURE_2D, 0,
            textureFormatToGLFormat(d.format),
            d.width, d.height, 0,
            textureFormatToGLBaseFormat(d.format),
            textureFormatToGLUploadFormat(d.format), init);

        if ( init != nullptr && d.mipMode == TextureMipMode::AutoGenerate )
            glGenerateMipmap(GL_TEXTURE_2D);
        else {
            // Reasonable defaults (you’ll likely use separate samplers anyway)
            glTextureParameteri(h, GL_TEXTURE_COMPARE_MODE, GL_NONE); // Required for Shadows
            glTextureParameteri(h, GL_TEXTURE_MIN_FILTER, d.mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glTextureParameteri(h, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(h, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(h, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

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
            GL_TEXTURE_2D, 0,
            textureFormatToGLFormat(d.format),
            newW, newH, 0,
            textureFormatToGLBaseFormat(d.format),
            textureFormatToGLUploadFormat(d.format), nullptr);

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

    void LGLTextureFactory::bind(TextureHandle h, uint32_t textureUnit) {
        // Validate texture unit against device limits to avoid GL_INVALID_ENUM.
        GLint maxUnits = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
        if ( static_cast<GLint>(textureUnit) >= maxUnits ) {
            std::cout << "WARNING::RENDERSYSTEM::TEXTUREFACTORY::BIND::INVALID_TEXTURE_UNIT" << std::endl;
            return;
        }
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        // always 2D textures for now
        glBindTexture(GL_TEXTURE_2D, h);
    }

} // namespace LaurelEye::Graphics
