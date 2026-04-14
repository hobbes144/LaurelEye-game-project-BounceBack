#include "LaurelEyeEngine/io/importers/JsonImporter.h"
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> JsonImporter::import(const std::string& path) {
        // --- Open the file in binary mode so we can detect BOM correctly ---
        std::ifstream file(path, std::ios::binary);
        if ( !file.is_open() ) {
            LE_ERROR("io", "[JsonImporter] Failed to open file: " << path);
            return nullptr;
        }

        // --- Detect and skip UTF-8 BOM (0xEF, 0xBB, 0xBF) ---
        unsigned char bom[3];
        file.read(reinterpret_cast<char*>(bom), 3);
        if ( !(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) ) {
            // Not a BOM, rewind
            file.seekg(0);
        }
        else {
            LE_DEBUG_INFO("io", "[JsonImporter] Stripped UTF-8 BOM from: " << path);
        }

        // --- Parse using RapidJSON’s IStreamWrapper ---
        rapidjson::IStreamWrapper isw(file);
        auto asset = std::make_shared<JsonAsset>(path);
        asset->jsonDocument.ParseStream(isw);

        // --- Handle parse errors with details ---
        if ( asset->jsonDocument.HasParseError() ) {
            LE_ERROR("io", "[JsonImporter] Parse error in: " << path << "\n"
                      << " -> " << rapidjson::GetParseError_En(asset->jsonDocument.GetParseError())
                      << " (offset " << asset->jsonDocument.GetErrorOffset() << ")");
            return nullptr;
        }

        return asset;
    }
} // namespace LaurelEye::IO
