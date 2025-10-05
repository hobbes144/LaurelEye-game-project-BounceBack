
#pragma once
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "LaurelEyeEngine/io/IAssetImporter.h"
#include "LaurelEyeEngine/io/Assets.h"

namespace LaurelEye::IO {
    class ShaderImporter : public IAssetImporter {
    public:
        std::shared_ptr<IAsset> import(const std::string& path) override;

    private:
        ShaderAsset::Stage detectStage(const IAsset& asset);
    };
} // namespace LaurelEye::IO
