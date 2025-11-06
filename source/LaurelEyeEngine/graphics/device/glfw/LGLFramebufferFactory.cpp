#include "LaurelEyeEngine/graphics/device/glfw/LGLFramebufferFactory.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

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

        for ( auto& attachment : r.desc.attachments ) {
            if ( attachment.size.width == 0 ) attachment.size = r.desc.size;
            attachTexture(r.id, attachment);
        }

        GLenum status = glCheckNamedFramebufferStatus(r.id, GL_FRAMEBUFFER);
        assert((status == GL_FRAMEBUFFER_COMPLETE) && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::CREATE::FAILED");

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
        assert(createdBuffers.contains(h) && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::ATTACH_TEXTURE::INVALID_FRAMEBUFFER");

        FramebufferAttachment a{};
        a.texture = d.texture;
        a.d = d;

        switch ( d.type ) {
        case FramebufferAttachmentType::Depth: {
            a.glAttachment = GL_DEPTH_ATTACHMENT;

            if ( isValidTexture(a.texture) ) {
                glNamedFramebufferTexture(h, a.glAttachment, a.texture, 0);
            }

            GLuint rbo;
            glCreateRenderbuffers(1, &rbo);
            // TODO: Add stricter checking for format being a Depth type.
            glNamedRenderbufferStorage(rbo, textureFormatToGLFormat(a.d.format), d.size.width, d.size.height); // Configure renderbuffer storage
            glNamedFramebufferRenderbuffer(h, a.glAttachment, GL_RENDERBUFFER, rbo);

            createdBuffers[h].attachments.push_back(a);
            return 0;
        }
        case FramebufferAttachmentType::Color: {
            a.glAttachment = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
            ++colorAttachmentIndex;

            if ( !isValidTexture(a.texture) ) {
                a.texture = createColorAttachmentTexture(h, d);
                assert(isValidTexture(a.texture) && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::ATTACH_TEXTURE::TEXTURE_CREATION_FAILED");
            }

            glNamedFramebufferTexture(h, a.glAttachment, a.texture, 0);

            createdBuffers[h].attachments.push_back(a);
            return colorAttachmentIndex - 1;
        }
        case LaurelEye::Graphics::FramebufferAttachmentType::DepthStencil:
        case LaurelEye::Graphics::FramebufferAttachmentType::Stencil:
        default:
            assert(false && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::CREATE::INVALID_ATTACHMENT_TYPE");
        }

        // This should never happen, but is here as fallback
        a.glAttachment = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
        ++colorAttachmentIndex;

        glNamedFramebufferTexture(createdBuffers[h].id, a.glAttachment, a.texture, 0);

        createdBuffers[h].attachments.push_back(a);
        return colorAttachmentIndex - 1;
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
        TextureHandle texture = textureFactory->create(t);

        return texture;
    }

} // namespace LaurelEye::Graphics
