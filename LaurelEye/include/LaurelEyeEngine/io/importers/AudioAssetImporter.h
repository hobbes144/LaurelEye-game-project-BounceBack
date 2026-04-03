/// @file AudioAssetImporter.h
#pragma once
#include "LaurelEyeEngine/audio/FModAudioManager.h"
#include "LaurelEyeEngine/io/IAssetImporter.h"

namespace LaurelEye::IO {

    enum class LoadHint {
        Sync,  // load immediately on import (for small files like sound effects)
        Async  // load in background after import (for large files like music)
    };

    class AudioAssetImporter : public IAssetImporter {
    public:
        explicit AudioAssetImporter(Audio::FModAudioManager& backend,
                                    LoadHint hint = LoadHint::Sync);

        std::shared_ptr<IAsset> import(const std::string& path) override;

    private:
        Audio::FModAudioManager& backend;
        LoadHint hint;
    };

} // namespace LaurelEye::IO
