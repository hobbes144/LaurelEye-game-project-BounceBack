#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "LaurelEyeEngine/io/importers/ImageImporter.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

#include <filesystem>

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> ImageImporter::import(const std::string& path) {
        if ( stbi_is_hdr(path.c_str()) ) {
            return loadHDRFile(path);
        }
        else {
            return loadSDRFile(path);
        }
    }

    std::shared_ptr<IAsset> ImageImporter::loadHDRFile(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        int channels = 0;
        int w = 0, h = 0;
        // Request 4 components (RGBA float)
        float* data = stbi_loadf(path.c_str(), &w, &h, &channels, 4);
        LE_ASSERT("io", data, "ERROR::TEXTURE::LOADHDR::LOADFAILED::" << path);

        Graphics::TextureDesc desc = Graphics::TextureDesc();
        desc.width = static_cast<uint32_t>(w);
        desc.height = static_cast<uint32_t>(h);
        desc.format = Graphics::TextureFormat::RGBA32F;

        auto asset = std::make_shared<ImageAsset>(path);
        asset->width = w;
        asset->height = h;
        asset->channels = 4;
        asset->format = ImageAsset::HDR_FLOAT;

        stbi_image_free(data);

        return asset;
    }

    std::shared_ptr<IAsset> ImageImporter::loadSDRFile(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        int channels = 0;
        int w = 0, h = 0;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
        LE_ASSERT("io", data, "ERROR::TEXTURE::LOADHDR::LOADFAILED::" << path);

        Graphics::TextureDesc desc = Graphics::TextureDesc();
        desc.width = static_cast<uint32_t>(w);
        desc.height = static_cast<uint32_t>(h);

        if ( channels == 3 )
            desc.format = Graphics::TextureFormat::RGB8;
        else if ( channels == 4 )
            desc.format = Graphics::TextureFormat::RGBA8;

        auto asset = std::make_shared<ImageAsset>(path);
        asset->width = w;
        asset->height = h;
        asset->channels = channels;
        asset->format = (channels == 3) ? ImageAsset::RGB8 : ImageAsset::RGBA8;

        // store raw pixels in case other systems need them
        size_t size = static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(channels);
        asset->pixelData.assign(data, data + size);

        stbi_image_free(data);

        return asset;
    }
} // namespace LaurelEye::IO
