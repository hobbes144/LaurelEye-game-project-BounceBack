#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "LaurelEyeEngine/io/importers/ImageImporter.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/io/Assets.h"

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

        stbi_image_free(data);
        return asset;
    }

    std::shared_ptr<IAsset> ImageImporter::loadHDRFile(const std::string& path) {
        /*TextureParameters textureParameters = TextureParameters(10,
                                                                TEXTURE_NEAREST, TEXTURE_LINEAR_MIPMAP_LINEAR,
                                                                TEXTURE_REPEAT, TEXTURE_REPEAT);*/
        Graphics::TextureDesc desc = Graphics::TextureDesc();

        stbi_set_flip_vertically_on_load(true);
        int channels;
        int w = static_cast<int>(desc.width);
        int h = static_cast<int>(desc.height);
        float* data = stbi_loadf(path.c_str(), &w, &h, &channels, 4);

        assert(("ERROR::TEXTURE::LOADTEXTURE::LOADFAILED::" + path).c_str() && data);

        desc.format = Graphics::TextureFormat::RGBA32F;

        Graphics::TextureHandle id = rs->createTexture("NamePlaceholder", desc, "TagPlaceholder", data);
        auto asset = std::make_shared<ImageAsset>(path);
        stbi_image_free(data);

        return asset;
    }

    std::shared_ptr<IAsset> ImageImporter::loadSDRFile(const std::string& path) {
        /*TextureParameters textureParameters = TextureParameters(10,
                                                                TEXTURE_NEAREST, TEXTURE_LINEAR_MIPMAP_LINEAR,
                                                                TEXTURE_REPEAT, TEXTURE_REPEAT);*/
        Graphics::TextureDesc desc = Graphics::TextureDesc();

        stbi_set_flip_vertically_on_load(true);
        int channels;
        int w = static_cast<int>(desc.width);
        int h = static_cast<int>(desc.height);
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);

        assert(("ERROR::TEXTURE::LOADTEXTURE::LOADFAILED::" + path).c_str() && data);

        if ( channels == 3 )
            desc.format = Graphics::TextureFormat::RGB8;
        else if ( channels == 4 )
            desc.format = Graphics::TextureFormat::RGBA8;

        Graphics::TextureHandle id = rs->createTexture("NamePlaceholder", desc, "TagPlaceholder", data);
        auto asset = std::make_shared<ImageAsset>(path);
        stbi_image_free(data);

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
