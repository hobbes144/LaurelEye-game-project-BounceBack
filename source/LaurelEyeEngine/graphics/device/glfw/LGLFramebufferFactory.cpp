#include "LaurelEyeEngine/graphics/device/glfw/LGLFramebufferFactory.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/surface/IWindowSurfaceProvider.h"

namespace LaurelEye::Graphics {

    LGLFramebufferFactory::LGLFramebufferFactory(LGLTextureFactory* textureFactory) {
        this->textureFactory = textureFactory;
    }

    FramebufferHandle LGLFramebufferFactory::create(const FramebufferDesc& d) {
        LGLFramebufferRecord r{};

        r.desc = d;

        glCreateFramebuffers(1, &r.id);

        for ( const auto& attachment : d.attachments ) {
            FramebufferAttachment tempAttachment;
            tempAttachment.d = attachment;
            switch ( attachment.type ) {
            case FramebufferAttachmentType::Depth: {
                r.attachments.push_back(createDepthAttachment(r.id, d.size, attachment));
                break;
            }
            case FramebufferAttachmentType::Color: {
                r.attachments.push_back(createColorAttachment(r.id, d.size, attachment));
                break;
            }
            case LaurelEye::Graphics::FramebufferAttachmentType::DepthStencil:
            case LaurelEye::Graphics::FramebufferAttachmentType::Stencil:
            default:
                assert(false && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::CREATE::INVALID_ATTACHMENT_TYPE");
            }
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

    void LGLFramebufferFactory::bindBase(FramebufferHandle h) {
        glBindFramebuffer(GL_FRAMEBUFFER, h);
    }

    uint32_t LGLFramebufferFactory::attachTexture(FramebufferHandle h, const FramebufferAttachmentDesc& d) {
        assert(createdBuffers.contains(h) && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::ATTACH_TEXTURE::INVALID_FRAMEBUFFER");
        assert(isValidTexture(d.texture) && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::ATTACH_TEXTURE::INVALID_TEXTURE");

        FramebufferAttachment a{};
        a.texture = d.texture;

        switch ( d.type ) {
        case FramebufferAttachmentType::Depth: {
            a.glAttachment = GL_DEPTH_ATTACHMENT;

            glNamedFramebufferTexture(createdBuffers[h].id, a.glAttachment, a.texture, 0);

            createdBuffers[h].attachments.push_back(a);
            return 0;
        }
        case FramebufferAttachmentType::Color: {
            a.glAttachment = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
            ++colorAttachmentIndex;

            glNamedFramebufferTexture(createdBuffers[h].id, a.glAttachment, a.texture, 0);

            createdBuffers[h].attachments.push_back(a);
            return colorAttachmentIndex-1;
        }
        case LaurelEye::Graphics::FramebufferAttachmentType::DepthStencil:
        case LaurelEye::Graphics::FramebufferAttachmentType::Stencil:
        default:
            assert(false && "ERROR::RENDER_SYSTEM::FRAMEBUFFER_FACTORY::CREATE::INVALID_ATTACHMENT_TYPE");
        }
    }

    FramebufferAttachment LGLFramebufferFactory::createDepthAttachment(
        GLuint framebufferID, const SizeRegistry& size, const FramebufferAttachmentDesc& d) {
        FramebufferAttachment a;
        a.glAttachment = GL_DEPTH_ATTACHMENT;

        if ( isValidTexture(d.texture) ) {
            glNamedFramebufferTexture(framebufferID, a.glAttachment, a.texture, 0);
            return a;
        }

        TextureDesc t{};
        t.type = TextureType::Texture2D;
        t.format = d.format;
        t.width = size.width;
        t.height = size.height;
        t.mipLevels = d.mipLevel;
        a.texture = textureFactory->create(t);

        glNamedFramebufferTexture(framebufferID, a.glAttachment, a.texture, 0);
        return a;
    }

    FramebufferAttachment LGLFramebufferFactory::createColorAttachment(
        GLuint framebufferID, const SizeRegistry& size, const FramebufferAttachmentDesc& d) {
        FramebufferAttachment a;
        a.glAttachment = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
        ++colorAttachmentIndex;

        if ( isValidTexture(d.texture) ) {
            glNamedFramebufferTexture(framebufferID, a.glAttachment, a.texture, d.mipLevel);
            return a;
        }

        TextureDesc t{};
        t.type = TextureType::Texture2D;
        t.format = d.format;
        t.width = size.width;
        t.height = size.height;
        t.mipLevels = d.mipLevel;
        a.texture = textureFactory->create(t);

        glNamedFramebufferTexture(framebufferID, a.glAttachment, a.texture, d.mipLevel);
        return a;
    }

} // namespace LaurelEye::Graphics
