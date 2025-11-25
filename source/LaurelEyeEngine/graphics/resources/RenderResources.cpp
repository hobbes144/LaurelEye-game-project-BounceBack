/// @file   RenderResources.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderResources

#include "LaurelEyeEngine/graphics/resources/RenderResources.h"

#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/resources/VertexArray.h"
#include "LaurelEyeEngine/graphics/surface/IWindowSurfaceProvider.h"

namespace LaurelEye::Graphics {

    RenderResources::RenderResources(IRenderDevice& dev, RenderSystem* rs) : device(dev) {
        // TODO: Holy crap this is bad. I'll figure it out later.
        rs->registerResizeCallback(0, [this](const SizeRegistry& size) {
            this->setSurfaceSize(0, size);
            this->resizeScreenSize(0, size);
        });
    }

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

    // DataBuffers

    DataBufferHandle RenderResources::createDataBuffer(
        const std::string& name, const DataBufferDesc& d, const std::vector<std::string>& tags, const void* init) {
        if ( dataBuffers.contains(name) ) return dataBuffers[name].handle;
        DataBufferResource r{};
        r.handle = device.createDataBuffer(d, init);
        r.desc = d;
        r.tags = tags;
        // r.lifetime = l;
        dataBuffers[name] = r;

        return r.handle;
    }

    DataBufferHandle RenderResources::createDataBuffer(const std::string& name, const DataBufferDesc& d,
                                                       const std::string& tag, const void* init) {
        return createDataBuffer(name, d, std::vector<std::string>{tag}, init);
    }

    DataBufferHandle RenderResources::dataBuffer(const std::string& name) {
        return dataBuffers[name].handle;
    }

    void RenderResources::destroyDataBuffer(const std::string& name) {
        DataBufferResource& buf = dataBuffers[name];
        device.destroyDataBuffer(buf.handle);
        dataBuffers.erase(name);
    }

    void RenderResources::destroyDataBuffer(DataBufferHandle h) {
        device.destroyDataBuffer(h);
        for ( auto it = dataBuffers.begin(); it != dataBuffers.end(); ) {
            if ( it->second.handle == h ) {
                it = dataBuffers.erase(it); // erase returns an iterator to the next element
                return;
            }
            else {
                ++it; // move to the next element
            }
        }
    }

    void RenderResources::updateDataBuffer(const std::string& name,
                                           uint64_t offset,
                                           uint64_t size,
                                           const void* data) {
        device.updateDataBufferSubData(dataBuffers[name].handle, offset, size, data);
    }

    void RenderResources::updateDataBuffer(DataBufferHandle h,
                                           uint64_t offset,
                                           uint64_t size,
                                           const void* data) {
        device.updateDataBufferSubData(h, offset, size, data);
    }

    // Vertex Arrays
    VertexArrayHandle RenderResources::createVertexArray(const std::string& name, const VertexArrayDesc& d,
                                                         const std::vector<std::string>& tags) {
        if ( vertexArrays.contains(name) ) return vertexArrays[name].handle;

        VertexArrayResource r{};
        r.desc = d;
        r.handle = device.createVertexArray(d);

        r.tags = tags;
        vertexArrays[name] = r;
        return r.handle;
    }

    // Textures

    TextureHandle RenderResources::createTexture(const std::string& name, const TextureDesc& d,
                                                 const std::vector<std::string>& tags, const void* init) {
        // if ( textures.contains(name) ) {
        //     // Destroy existing before recreating to avoid leaks or name collision
        //     destroyTexture(name);
        // }
        if ( textures.contains(name) ) return textures[name].handle;

        TextureResource r{};
        r.desc = d;
        if ( d.width == 0 ) {
            r.desc.width = size.width;
            r.desc.height = size.height;
            r.tags.push_back("screen_size");
        }
        r.handle = device.createTexture(r.desc, init);

        r.tags = tags;
        textures[name] = r;
        return r.handle;
    }

    TextureHandle RenderResources::createTexture(const std::string& name, const TextureDesc& d,
                                                 const std::string& tag, const void* init) {
        return createTexture(name, d, std::vector<std::string>{tag}, init);
    }

    TextureHandle RenderResources::createScreenSizeTexture(const std::string& name,
                                                           const std::vector<std::string>& tags,
                                                           const SampleCount& samples) {
        TextureDesc d;
        d.mipMode = TextureMipMode::None;
        d.type = TextureType::Texture2D;
        d.width = 0; // Setting as screen_size.
        d.debugName = name;
        d.format = TextureFormat::RGBA32F;
        d.samples = samples;
        return createTexture(name, d, tags);
    }

    TextureHandle RenderResources::texture(const std::string& name) {
        if ( !textures.contains(name) ) return InvalidTexture;
        return textures[name].handle;
    }

    void RenderResources::destroyTexture(const std::string& name) {
        auto it = textures.find(name);
        if ( it == textures.end() ) return;

        device.destroyTexture(it->second.handle);
        textures.erase(it);
    }

    void RenderResources::resizeTexture(const std::string& name, const SizeRegistry& size) {
        device.resizeTexture(textures[name].handle, size.width, size.height);
    }

    // Framebuffers

    FramebufferHandle RenderResources::createFramebuffer(const std::string& name, const FramebufferDesc& d,
                                                         const std::vector<std::string>& tags) {
        if ( framebuffers.contains(name) ) return framebuffers[name].handle;
        FramebufferResource r{};
        r.desc = d;
        r.tags = tags;

        if ( d.size.width == 0 ) {
            r.desc.size = size;
            r.tags.push_back("screen_size");
        }

        FramebufferAttachmentIndex attachmentIndex = 0;
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
                attachment.texture = createTexture(name + "_" + std::to_string(attachmentIndex), t, tags);
                attachment.colorAttachmentIndex = attachmentIndex;
                ++attachmentIndex;
            }
            // NOTE: Using mixed defined and undefined attachment indices is not tested.
            // attachmentIndex = std::max(attachmentIndex, attachment.colorAttachmentIndex);
        }

        // Separated out texture creation.
        // This allows textures to be created and managed by RenderResources
        // instead of the TextureManager only.
        r.handle = device.createFramebuffer(r.desc);
        // r.lifetime = l;
        framebuffers[name] = r;

        return r.handle;
    }

    FramebufferHandle RenderResources::createFramebuffer(const std::string& name, const FramebufferDesc& d,
                                                         const std::string& tag) {
        return createFramebuffer(name, d, std::vector<std::string>{tag});
    }

    TextureHandle RenderResources::createScreenSizeFramebuffer(const std::string& name,
                                                               const std::vector<std::string>& textureNames,
                                                               const std::vector<std::string>& tags,
                                                               const SampleCount& samples) {
        FramebufferDesc d;
        d.size.width = 0; // Setting as screen_size.
        FramebufferAttachmentIndex attachmentIndex = 0;
        for ( const auto& textureName : textureNames ) {
            FramebufferAttachmentDesc a;
            a.type = FramebufferAttachmentType::Color;
            a.texture = createScreenSizeTexture(textureName, tags, samples);
            a.colorAttachmentIndex = attachmentIndex;
            d.attachments.push_back(a);
            ++attachmentIndex;
        }

        FramebufferAttachmentDesc a;
        a.type = FramebufferAttachmentType::Depth;
        // a.format = TextureFormat::DEPTH32F;
        a.format = TextureFormat::DEPTH24;
        a.samples = samples;
        a.size.width = 0;
        d.attachments.push_back(a);

        return createFramebuffer(name, d, tags);
    }

    FramebufferHandle RenderResources::framebuffer(const std::string& name) {
        return framebuffers[name].handle;
    }

    void RenderResources::destroyFramebuffer(const std::string& name) {
        // NOTE: This call will also update the textures attached to it.
        FramebufferResource& fbo = framebuffers[name];
        device.destroyFramebuffer(fbo.handle);
        framebuffers.erase(name);
    }

    void RenderResources::resizeFramebuffer(const std::string& name, const SizeRegistry& size) {
        device.resizeFramebuffer(framebuffers[name].handle, size.width, size.height);
    }

    // General resource interactions

    void RenderResources::resizeTag(const std::string& tag, const SizeRegistry& size) {
        // Later add code to resize data buffers.
        for ( const auto& [name, texture] : textures ) {
            if ( std::find(texture.tags.begin(), texture.tags.end(), tag) != texture.tags.end() ) {
                resizeTexture(name, size);
            }
        }
        for ( const auto& [name, framebuffer] : framebuffers ) {
            if ( std::find(framebuffer.tags.begin(), framebuffer.tags.end(), tag) != framebuffer.tags.end() ) {
                resizeFramebuffer(name, size);
            }
        }
    }

    void RenderResources::resizeScreenSize(SurfaceHandle h, const SizeRegistry& size) {
        resizeTag("screen_size", size);
    }

    // RenderDevice

    IRenderDevice& RenderResources::getRenderDevice() {
        return device;
    }

    void RenderResources::setSurfaceSize(SurfaceHandle h, const SizeRegistry& size) {
        this->size = size;
    }

    const SizeRegistry& RenderResources::getSurfaceSize(SurfaceHandle h) const {
        return size;
    }

} // namespace LaurelEye::Graphics
