/// @file   LGLFramebufferFactory.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-06-2025
/// @brief Framebuffer Factory for OpenGL rendering

#include "LaurelEyeEngine/graphics/device/glfw/LGLFramebufferFactory.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

#include "LaurelEyeEngine/logging/EngineLog.h"
#include <vector>

namespace LaurelEye::Graphics {

    LGLFramebufferFactory::LGLFramebufferFactory(LGLTextureFactory* textureFactory) {
        this->textureFactory = textureFactory;
    }

    LGLFramebufferFactory::~LGLFramebufferFactory() {
        destroyAll();
    }

    FramebufferHandle LGLFramebufferFactory::create(const FramebufferDesc& d) {
        LGLFramebufferRecord r{};

        r.desc = d;

        glCreateFramebuffers(1, &r.id);
        createdBuffers[r.id] = r;

        std::vector<GLenum> bufferAttachments;
        for ( auto& attachment : r.desc.attachments ) {
            if ( attachment.size.width == 0 ) attachment.size = r.desc.size;
            attachTexture(r.id, attachment);
            if ( attachment.type == FramebufferAttachmentType::Color )
                bufferAttachments.push_back(GL_COLOR_ATTACHMENT0 + attachment.colorAttachmentIndex);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, r.id);
        glDrawBuffers((GLsizei)bufferAttachments.size(), bufferAttachments.data());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLenum status = glCheckNamedFramebufferStatus(r.id, GL_FRAMEBUFFER);
        LE_ASSERT("Graphics", status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer Factory creation failed.");

        return r.id;
    }

    void LGLFramebufferFactory::destroy(FramebufferHandle h) {
        LGLFramebufferRecord& fbo = createdBuffers[h];

        glDeleteFramebuffers(1, &fbo.id);

        for ( int i = 0; i < fbo.attachments.size(); ++i ) {
            textureFactory->destroy(fbo.attachments[i].texture);
        }

        createdBuffers.erase(h);
    }

    void LGLFramebufferFactory::destroyAll() {
        for ( auto& [h, d] : createdBuffers ) {
            glDeleteFramebuffers(1, &createdBuffers[h].id);
            for ( int i = 0; i < createdBuffers[h].attachments.size(); ++i ) {
                textureFactory->destroy(createdBuffers[h].attachments[i].texture);
            }
        }
        createdBuffers.clear();
    }

    void LGLFramebufferFactory::bindBase(FramebufferHandle h) {
        glBindFramebuffer(GL_FRAMEBUFFER, h);
    }

    uint32_t LGLFramebufferFactory::attachTexture(FramebufferHandle h, const FramebufferAttachmentDesc& d) {
        LE_ASSERT("Graphics", createdBuffers.contains(h), "Invalid Framebuffer.");

        FramebufferAttachment a{};
        a.texture = d.texture;
        a.d = d;

        switch ( d.type ) {
        case FramebufferAttachmentType::Depth: {
            a.glAttachment = GL_DEPTH_ATTACHMENT;

            if ( isValidTexture(a.texture) ) {
                glNamedFramebufferTexture(h, a.glAttachment, a.texture, 0);
            }

            glCreateRenderbuffers(1, &rbo);
            if ( a.d.samples == SampleCount::X1 ) {
                // TODO: Add stricter checking for format being a Depth type.
                glNamedRenderbufferStorage(rbo, textureFormatToGLFormat(a.d.format), d.size.width, d.size.height); // Configure renderbuffer storage
            }
            else {
                glNamedRenderbufferStorageMultisample(rbo, (GLsizei)getSampleIntFromSampleCount(a.d.samples), textureFormatToGLFormat(a.d.format), d.size.width, d.size.height);
            }
            glNamedFramebufferRenderbuffer(h, a.glAttachment, GL_RENDERBUFFER, rbo);

            createdBuffers[h].attachments.push_back(a);
            return 0;
        }
        case FramebufferAttachmentType::Color: {
            a.glAttachment = GL_COLOR_ATTACHMENT0 + a.d.colorAttachmentIndex;

            if ( !isValidTexture(a.texture) ) {
                a.texture = createColorAttachmentTexture(h, d);
                LE_ASSERT("Graphics", isValidTexture(a.texture), "Texture creation failed.");
            }

            glNamedFramebufferTexture(h, a.glAttachment, a.texture, 0);

            createdBuffers[h].attachments.push_back(a);
            return a.d.colorAttachmentIndex;
        }
        case LaurelEye::Graphics::FramebufferAttachmentType::DepthStencil:
        case LaurelEye::Graphics::FramebufferAttachmentType::Stencil:
        default:
            LE_FATAL_ERROR("Graphics", "Invalid attachment type.");
        }

        // This should never happen, but is here as fallback
        a.glAttachment = GL_COLOR_ATTACHMENT0 + a.d.colorAttachmentIndex;

        glNamedFramebufferTexture(createdBuffers[h].id, a.glAttachment, a.texture, 0);

        createdBuffers[h].attachments.push_back(a);
        return a.d.colorAttachmentIndex;
    }

    TextureHandle LGLFramebufferFactory::createColorAttachmentTexture(
        GLuint framebufferID, const FramebufferAttachmentDesc& d) {
        if ( isValidTexture(d.texture) ) {
            return d.texture;
        }

        TextureDesc t{};
        t.type = TextureType::Texture2D;
        t.format = d.format;
        t.width = d.size.width;
        t.height = d.size.height;
        t.mipLevels = d.mipLevels;
        t.samples = d.samples;
        TextureHandle texture = textureFactory->create(t);

        return texture;
    }

    void LGLFramebufferFactory::resize(FramebufferHandle h, uint32_t width, uint32_t height) {
        LGLFramebufferRecord& framebuffer = createdBuffers[h];
        framebuffer.desc.size.width = width;
        framebuffer.desc.size.height = height;

        for ( auto& attachment : framebuffer.attachments ) {
            attachment.d.size = framebuffer.desc.size;
            if ( isValidTexture(attachment.texture) )
                textureFactory->resize(attachment.texture, framebuffer.desc.size.width, framebuffer.desc.size.height);
            if ( attachment.d.type == FramebufferAttachmentType::Depth ) {
                if ( rbo ) glDeleteRenderbuffers(1, &rbo);
                glCreateRenderbuffers(1, &rbo);
                if ( attachment.d.samples == SampleCount::X1 ) {
                    glNamedRenderbufferStorage(rbo, textureFormatToGLFormat(attachment.d.format), width, height);
                }
                else {
                    glNamedRenderbufferStorageMultisample(rbo, (GLsizei)getSampleIntFromSampleCount(attachment.d.samples), textureFormatToGLFormat(attachment.d.format), attachment.d.size.width, attachment.d.size.height);
                }
                glNamedFramebufferRenderbuffer(h, attachment.glAttachment, GL_RENDERBUFFER, rbo);
            }
        }
    }

    void LGLFramebufferFactory::blit(FramebufferHandle source, FramebufferHandle dest, bool depthOnly) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, source);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest);

        for ( const auto& attachment : createdBuffers[source].attachments ) {
            GLenum att = attachment.glAttachment;

            if ( attachment.d.type == FramebufferAttachmentType::Depth ) {
                if ( !depthOnly ) {
                    glReadBuffer(GL_NONE);
                    glDrawBuffer(GL_NONE);
                }
                glBlitFramebuffer(
                    0, 0, attachment.d.size.width, attachment.d.size.height,
                    0, 0, attachment.d.size.width, attachment.d.size.height,
                    GL_DEPTH_BUFFER_BIT,
                    GL_NEAREST // required for MSAA resolve
                );
            }
            else if ( !depthOnly ) {
                glReadBuffer(att);
                glDrawBuffer(att);
                glBlitFramebuffer(
                    0, 0, attachment.d.size.width, attachment.d.size.height,
                    0, 0, attachment.d.size.width, attachment.d.size.height,
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST // required for MSAA resolve
                );
            }
        }
    }

} // namespace LaurelEye::Graphics
