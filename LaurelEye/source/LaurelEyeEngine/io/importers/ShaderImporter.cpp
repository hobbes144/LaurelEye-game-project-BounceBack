#include "LaurelEyeEngine/io/importers/ShaderImporter.h"

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> ShaderImporter::import(const std::string& path) {
        auto asset = std::make_shared<ShaderAsset>(path);

        ShaderAsset::Stage stage = detectStage(path);
        if ( stage == ShaderAsset::Stage::Unknown ) {
            std::cerr << "Failed to load Shader at path: " << path << std::endl;
            return nullptr;
        }
        std::ifstream file(path);
        if ( !file.is_open() )
            throw std::runtime_error("ShaderImporter: Failed to open " + path);

        std::stringstream buffer;
        buffer << file.rdbuf();

        asset->sources[stage] = buffer.str();
        return asset;
    }

    ShaderAsset::Stage ShaderImporter::detectStage(const IAsset& asset) {
        const auto& ext = asset.getExtension();
        if ( ext == "vert" ) return ShaderAsset::Stage::Vertex;
        if ( ext == "frag" ) return ShaderAsset::Stage::Fragment;
        if ( ext == "geom" ) return ShaderAsset::Stage::Geometry;
        if ( ext == "tesc" ) return ShaderAsset::Stage::TessControl;
        if ( ext == "tese" ) return ShaderAsset::Stage::TessEval;
        if ( ext == "comp" ) return ShaderAsset::Stage::Compute;

        return ShaderAsset::Stage::Unknown;
    }
} // namespace LaurelEye::IO

