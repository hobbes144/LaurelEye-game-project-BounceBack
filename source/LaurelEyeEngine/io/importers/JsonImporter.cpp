#include "LaurelEyeEngine/io/importers/JsonImporter.h"

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> JsonImporter::import(const std::string& path) {
        std::ifstream file(path);
        if ( !file.is_open() ) return nullptr;

        std::stringstream buffer;
        buffer << file.rdbuf();

        auto asset = std::make_shared<JsonAsset>(path);
        asset->jsonDocument.Parse(buffer.str().c_str());

        if ( asset->jsonDocument.HasParseError() )
            return nullptr;

        return asset;
    }
} // namespace LaurelEye::IO 
