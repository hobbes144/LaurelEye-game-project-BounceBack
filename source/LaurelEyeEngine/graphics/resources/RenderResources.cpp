/// @file   RenderResources.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderResources

#include "LaurelEyeEngine/graphics/resources/RenderResources.h"

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

    // TextureHandle RenderResources::createTexture(const std::string& name, const TextureDesc& d,
    //                                              const std::string& tag) {
    // }

    DataBufferHandle RenderResources::dataBuffer(const std::string& name) {
        return databuffers[name].handle;
    }
    void RenderResources::destroyDataBuffer(const std::string& name) {
        DataBufferResource& buf = databuffers[name];
        device.destroyDataBuffer(buf.handle);
        databuffers.erase(name.data());
    }

} // namespace LaurelEye::Graphics
