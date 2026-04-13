#include "LaurelEyeEngine/io/importers/JsonImporter.h"
#include "LaurelEyeEngine/io/Assets.h"

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> JsonImporter::import(const std::string& path) {
        // --- Open the file in binary mode so we can detect BOM correctly ---
        std::ifstream file(path, std::ios::binary);
        if ( !file.is_open() ) {
            std::cerr << "[JsonImporter] Failed to open file: " << path << std::endl;
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
            std::cout << "[JsonImporter] Stripped UTF-8 BOM from: " << path << std::endl;
        }

        // --- Parse using RapidJSON’s IStreamWrapper ---
        rapidjson::IStreamWrapper isw(file);
        auto asset = std::make_shared<JsonAsset>(path);
        asset->jsonDocument.ParseStream(isw);

        // --- Handle parse errors with details ---
        if ( asset->jsonDocument.HasParseError() ) {
            std::cerr << "[JsonImporter] Parse error in: " << path << "\n"
                      << " -> " << rapidjson::GetParseError_En(asset->jsonDocument.GetParseError())
                      << " (offset " << asset->jsonDocument.GetErrorOffset() << ")\n";
            return nullptr;
        }

        return asset;
    }
} // namespace LaurelEye::IO
