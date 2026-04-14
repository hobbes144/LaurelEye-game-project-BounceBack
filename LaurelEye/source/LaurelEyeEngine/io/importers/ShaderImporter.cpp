#include "LaurelEyeEngine/io/importers/ShaderImporter.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> ShaderImporter::import(const std::string& path) {
        auto asset = std::make_shared<ShaderAsset>(path);

        ShaderAsset::Stage stage = detectStage(path);
        if ( stage == ShaderAsset::Stage::Unknown ) {
            LE_ERROR("io", "Failed to load Shader at path: " << path);
            return nullptr;
        }
        std::ifstream file(path);
        LE_ASSERT("io", file.is_open(), "ShaderImporter: Failed to open " + path);

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

