/// @file   RenderResources.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderResources

#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"

namespace LaurelEye::Graphics {

    DataBufferHandle RenderResources::createDataBuffer(
        const std::string& name, const DataBufferDesc& d, const std::string& tag) {
        DataBufferResource r{};
        r.handle = device.createDataBuffer(d);
        r.desc = d;
        r.tag = tag;
        // r.lifetime = l;
        databuffers[name] = r;

        return r.handle;
    }

    TextureHandle RenderResources::createTexture(const std::string& name, const TextureDesc& d,
                                                 const std::string& tag, const void* init) {
        TextureResource r{};
        r.handle = device.createTexture(d, init);
        r.desc = d;
        r.tag = tag;
        textures[name] = r;
        return r.handle;
    }

    FramebufferHandle RenderResources::createFramebuffer(const std::string& name, const FramebufferDesc& d,
                                                         const std::string& tag) {
        FramebufferResource r{};
        r.desc = d;
        r.tag = tag;

        // Using temp so we separate out texture creation.
        // This allows textures to be created and managed by RenderResources
        // instead of the TextureManager only.
        FramebufferDesc temp = {{}, d.x, d.y, d.size};
        r.handle = device.createFramebuffer(temp);
        // r.lifetime = l;
        framebuffers[name] = r;
        int attachmentNumber = 0;
        for ( auto& attachment : r.desc.attachments ) {
            TextureDesc t{};
            t.type = TextureType::Texture2D;
            t.format = attachment.format;
            t.width = d.size.width;
            t.height = d.size.height;
            t.mipLevels = attachment.mipLevel;
            if ( attachment.type == FramebufferAttachmentType::Color ) {
                attachment.texture = createTexture(name + "_" + std::to_string(attachmentNumber), t, tag);
                attachmentNumber++;
            }
            else {
                attachment.texture = createTexture(name + "_depth", t, tag);
            }
            device.attachTexturetoFramebuffer(r.handle, attachment);
        }

        return r.handle;
    }

    DataBufferHandle RenderResources::dataBuffer(const std::string& name) {
        return databuffers[name].handle;
    }

    TextureHandle RenderResources::texture(const std::string& name) {
        if (!textures.contains(name)) return InvalidTexture;
        return textures[name].handle;
    }

    FramebufferHandle RenderResources::framebuffer(const std::string& name) {
        return framebuffers[name].handle;
    }

    void RenderResources::destroyDataBuffer(const std::string& name) {
        DataBufferResource& buf = databuffers[name];
        device.destroyDataBuffer(buf.handle);
        databuffers.erase(name);
    }

    void RenderResources::destroyTexture(const std::string& name) {
        TextureResource& tex = textures[name];
        device.destroyTexture(tex.handle);
        textures.erase(name.data());
    }

    void RenderResources::destroyFramebuffer(const std::string& name) {
        FramebufferResource& fbo = framebuffers[name];
        device.destroyFramebuffer(fbo.handle);
        framebuffers.erase(name);
    }

} // namespace LaurelEye::Graphics
