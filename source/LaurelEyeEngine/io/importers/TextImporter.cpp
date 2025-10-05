#include "LaurelEyeEngine/io/importers/TextImporter.h"

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> TextImporter::import(const std::string& path) {
        std::ifstream file(path);
        if ( !file.is_open() ) return nullptr;

        std::stringstream buffer;
        buffer << file.rdbuf();

        auto asset = std::make_shared<TextAsset>(path);
        asset->text = buffer.str();
        return asset;
    }
} // namespace LaurelEye::IO
