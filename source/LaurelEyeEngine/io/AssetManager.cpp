#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/io/FileSystem.h"
#include "LaurelEyeEngine/io/importers/MeshImporter.h"
#include "LaurelEyeEngine/io/importers/ImageImporter.h"
#include "LaurelEyeEngine/io/importers/JsonImporter.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"

namespace LaurelEye::IO {
    void AssetManager::initialize() {
        auto meshImporter = std::make_shared<IO::MeshImporter>();
        auto imgImporter = std::make_shared<IO::ImageImporter>();
        auto jsonImporter = std::make_shared<IO::JsonImporter>();

        // 3D models
        registerImporter("obj", meshImporter);
        registerImporter("fbx", meshImporter);

        assert(context && "AssetManager: No EngineContext provided. Cannot locate RenderSystem for texture importing.");

        registerImporter("png", imgImporter);
        registerImporter("jpg", imgImporter);
        registerImporter("jpeg", imgImporter);

        // Json
        registerImporter("json", std::make_unique<IO::JsonImporter>());
    }

    void AssetManager::registerImporter(const std::string& extension, std::shared_ptr<IAssetImporter> importer) {
        std::string lowerExt = normalizeExtension(extension);
        importers[lowerExt] = importer;
    }

    std::shared_ptr<IAsset> AssetManager::load(const std::string& path) {

        // Check if the data file needed has been cached already by using its path
        if ( auto cached = assetCache.find(path); cached != assetCache.end() ) {
            return cached->second;
        }
       
        // If it hasn't been used before, look for the importer to import it by its extension
        std::string ext = extractExtension(path);
        auto it = importers.find(ext);
        if ( it == importers.end() ) {
            throw std::runtime_error("No importer registered for extension: " + ext);
        }
        // Use this to always find the correct path to load (source based loading)
        auto fullPath = resolve(path);
        // Call the importer's import method
        auto asset = it->second->import(fullPath.string());
        if ( asset ) {
            assetCache[path] = asset;
        }
        return asset;
    }

    std::future<std::shared_ptr<IAsset>> AssetManager::loadAsync(const std::string& path) {
        return std::async(std::launch::async, [this, path]() {
            return this->load(path);
        });
    }

    std::vector<std::future<std::shared_ptr<IAsset>>> AssetManager::loadBatchAsync(const std::vector<std::string>& paths) {
        std::vector<std::future<std::shared_ptr<IAsset>>> futures;
        futures.reserve(paths.size());
        for ( const auto& path : paths ) {
            futures.emplace_back(loadAsync(path));
        }
        return futures;
    }

    void AssetManager::unload(const std::string& path) {
        std::unique_lock lock(cacheMutex);
        assetCache.erase(path);
    }

    std::string AssetManager::getExtension(const std::string& path) {
        auto dot = path.find_last_of('.');
        if ( dot == std::string::npos ) return "";
        std::string ext = path.substr(dot + 1);
        return normalizeExtension(ext);
    }

    std::string AssetManager::normalizeExtension(const std::string& ext) {
        std::string lower = ext;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return lower;
    }

} // namespace LaurelEye
