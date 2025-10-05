#include "LaurelEyeEngine/io/importers/ImageImporter.h"

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

        stbi_image_free(data);
        return asset;
    }
} // namespace LaurelEye::IO
