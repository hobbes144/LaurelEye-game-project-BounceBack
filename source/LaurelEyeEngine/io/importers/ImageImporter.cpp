#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "LaurelEyeEngine/io/importers/ImageImporter.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <filesystem>

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> ImageImporter::import(const std::string& path) {
        int w, h, c;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);
        if ( !data ) return nullptr;

        auto asset = std::make_shared<ImageAsset>(path);
        asset->width = w;
        asset->height = h;
        asset->channels = c;
        asset->pixelData.assign(data, data + (w * h * c));

        if ( stbi_is_hdr(path.c_str()) ) {
            return loadHDRFile(path);
        }
        else {
            return loadSDRFile(path);
        }
    }

    std::shared_ptr<IAsset> ImageImporter::loadHDRFile(const std::string& path) {
        /*TextureParameters textureParameters = TextureParameters(10,
                                                                TEXTURE_NEAREST, TEXTURE_LINEAR_MIPMAP_LINEAR,
                                                                TEXTURE_REPEAT, TEXTURE_REPEAT);*/
        stbi_set_flip_vertically_on_load(true);
        int channels = 0;
        int w = 0, h = 0;
        // Request 4 components (RGBA float)
        float* data = stbi_loadf(path.c_str(), &w, &h, &channels, 4);
        assert(("ERROR::TEXTURE::LOADHDR::LOADFAILED::" + path).c_str() && data);

        Graphics::TextureDesc desc = Graphics::TextureDesc();
        desc.width = static_cast<uint32_t>(w);
        desc.height = static_cast<uint32_t>(h);
        desc.format = Graphics::TextureFormat::RGBA32F;

        Graphics::TextureHandle id = rs->createTexture(path, desc, "ImageImporter", data);

        auto asset = std::make_shared<ImageAsset>(path);
        asset->width = w;
        asset->height = h;
        asset->channels = 4;
        asset->format = ImageAsset::HDR_FLOAT;

        stbi_image_free(data);

        (void)id;

        return asset;
    }

    std::shared_ptr<IAsset> ImageImporter::loadSDRFile(const std::string& path) {
        /*TextureParameters textureParameters = TextureParameters(10,
                                                                TEXTURE_NEAREST, TEXTURE_LINEAR_MIPMAP_LINEAR,
                                                                TEXTURE_REPEAT, TEXTURE_REPEAT);*/
        stbi_set_flip_vertically_on_load(true);
        int channels = 0;
        int w = 0, h = 0;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
        assert(("ERROR::TEXTURE::LOADSDR::LOADFAILED::" + path).c_str() && data);

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
        Graphics::TextureHandle id = rs->createTexture(asset->getName(), desc, "ImageImporter", data);

        // store raw pixels in case other systems need them
        asset->pixelData.assign(data, data + (w * h * channels));

        stbi_image_free(data);

        (void)id; // created GPU texture handle is intentionally not stored on ImageAsset for now

        return asset;
    }

    /*
    two types of textures we are importing: hdr and sdr
    Anish gave me all the code in messages for importing them both
    One line that does a check to see if its an hdr or sdr, the rest of the code is specific to each type

    Main difference is the image description
    above function (import) works with sdr, but not hdr
    import will call the two functions
    Have the functions sent be a part of image importer class as helper functions - make them private

    For each part of texture description, keep the line in from the helper functions
    If not in textureDesc, comment out line
    Main ones to set are width, height, and format
    This should call RenderResources.createtexture
    Figure out how RenderResources can be called in this
    Anish thinks it could be AssetManager.AddRenderwhateverthefuck


    */
} // namespace LaurelEye::IO
