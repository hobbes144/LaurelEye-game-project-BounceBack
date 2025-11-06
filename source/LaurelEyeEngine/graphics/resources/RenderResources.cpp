/// @file   RenderResources.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderResources

#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

namespace LaurelEye::Graphics {

    RenderResources::~RenderResources() {
        for ( auto& [name, desc] : dataBuffers ) {
            device.destroyDataBuffer(desc.handle);
        }
        dataBuffers.clear();

        for ( auto& [name, desc] : framebuffers ) {
            device.destroyFramebuffer(desc.handle);
        }
        framebuffers.clear();

        for ( auto& [name, desc] : textures ) {
            device.destroyTexture(desc.handle);
        }
        textures.clear();
    }

    DataBufferHandle RenderResources::createDataBuffer(
        const std::string& name, const DataBufferDesc& d, const std::string& tag) {
        if (dataBuffers.contains(name)) return dataBuffers[name].handle;
        DataBufferResource r{};
        r.handle = device.createDataBuffer(d);
        r.desc = d;
        r.tag = tag;
        // r.lifetime = l;
        dataBuffers[name] = r;

        return r.handle;
    }

    TextureHandle RenderResources::createTexture(const std::string& name, const TextureDesc& d,
                                                 const std::string& tag, const void* init) {
        // if ( textures.contains(name) ) {
        //     // Destroy existing before recreating to avoid leaks or name collision
        //     destroyTexture(name);
        // }
        if (textures.contains(name)) return textures[name].handle;

        TextureResource r{};
        r.handle = device.createTexture(d, init);
        r.desc = d;
        r.tag = tag;
        textures[name] = r;
        return r.handle;
    }

    FramebufferHandle RenderResources::createFramebuffer(const std::string& name, const FramebufferDesc& d,
                                                         const std::string& tag) {
        if (framebuffers.contains(name)) return framebuffers[name].handle;
        FramebufferResource r{};
        r.desc = d;
        r.tag = tag;
        int attachmentNumber = 0;
        for ( auto& attachment : r.desc.attachments ) {
            if ( attachment.texture == InvalidTexture &&
                 attachment.type == FramebufferAttachmentType::Color ) {
                TextureDesc t{};
                t.type = TextureType::Texture2D;
                t.format = attachment.format;
                t.width = d.size.width;
                t.height = d.size.height;
                t.mipMode = attachment.mipMode;
                t.mipLevels = attachment.mipLevels;
                attachment.texture = createTexture(name + "_" + std::to_string(attachmentNumber), t, tag);
                attachmentNumber++;
            }
        }

        // Separated out texture creation.
        // This allows textures to be created and managed by RenderResources
        // instead of the TextureManager only.
        r.handle = device.createFramebuffer(r.desc);
        // r.lifetime = l;
        framebuffers[name] = r;

        return r.handle;
    }

    DataBufferHandle RenderResources::dataBuffer(const std::string& name) {
        return dataBuffers[name].handle;
    }

    TextureHandle RenderResources::texture(const std::string& name) {
        if ( !textures.contains(name) ) return InvalidTexture;
        return textures[name].handle;
    }

    FramebufferHandle RenderResources::framebuffer(const std::string& name) {
        return framebuffers[name].handle;
    }

    void RenderResources::destroyDataBuffer(const std::string& name) {
        DataBufferResource& buf = dataBuffers[name];
        device.destroyDataBuffer(buf.handle);
        dataBuffers.erase(name);
    }

    void RenderResources::destroyTexture(const std::string& name) {
        auto it = textures.find(name);
        if ( it == textures.end() ) return;

        device.destroyTexture(it->second.handle);
        textures.erase(it);
    }

    void RenderResources::destroyFramebuffer(const std::string& name) {
        FramebufferResource& fbo = framebuffers[name];
        device.destroyFramebuffer(fbo.handle);
        framebuffers.erase(name);
    }

    IRenderDevice& RenderResources::getRenderDevice() {
        return device;
    }

} // namespace LaurelEye::Graphics
