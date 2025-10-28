/// @file   RenderResources.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderResources

#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

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

    DataBufferHandle RenderResources::dataBuffer(const std::string& name) {
        return databuffers[name].handle;
    }

    TextureHandle RenderResources::texture(const std::string& name) {
        if (!textures.contains(name)) return InvalidTexture;
        return textures[name].handle;
    }

    void RenderResources::destroyDataBuffer(const std::string& name) {
        DataBufferResource& buf = databuffers[name];
        device.destroyDataBuffer(buf.handle);
        databuffers.erase(name.data());
    }

    void RenderResources::destroyTexture(const std::string& name) {
        TextureResource& tex = textures[name];
        device.destroyTexture(tex.handle);
        textures.erase(name.data());
    }

} // namespace LaurelEye::Graphics
