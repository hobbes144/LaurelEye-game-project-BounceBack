#include "LaurelEyeEngine/scene/SceneManager.h"
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace LaurelEye {
    SceneManager::SceneManager(EngineContext& ctx, const EngineConfig& config) :
        context(&ctx), sceneListPath(""), currentScene(nullptr) {
        sceneListPath = config.initialSceneList;
        assetsRoot = config.assetRoot;
    }

    SceneManager::~SceneManager() {
        
        currentScene->shutdown(*context);
        scenes.clear();
        sceneFilePaths.clear();
        currentScene = nullptr;
        switchingScene = false;
    }

    void SceneManager::initialize() {
        sceneListDeserialize();

        if ( !initialSceneName.empty() && hasScene(initialSceneName) ) {
            changeScene(initialSceneName);
        }
        else if ( !scenes.empty() ) {
            // Fallback to first scene if default missing
            changeScene(scenes.begin()->first);
        }
    }

    void SceneManager::update(float deltaTime) {
        if ( switchingScene && !nextSceneQueued.empty() ) {
            performSceneSwitch();
        }

        if ( currentScene ) {
            currentScene->update(deltaTime);
        }
    }

    void SceneManager::changeScene(const std::string& nextScene) {
        if ( !hasScene(nextScene) ) {
            throw std::runtime_error("SceneManager: Unknown scene '" + nextScene + "'");
        }
        nextSceneQueued = nextScene;
        switchingScene = true;
    }

    void SceneManager::reloadCurrentScene() {
        if ( !currentScene ) return;

        std::string sceneName = currentScene->getName();
        currentScene->shutdown(*context);
        currentScene->initialize(*context);
    }

    bool SceneManager::hasScene(const std::string& name) const {
        return scenes.find(name) != scenes.end();
    }

     Scene* SceneManager::getScene(const std::string& name) {
        auto it = scenes.find(name);
        if ( it != scenes.end() )
            return it->second.get();
        return nullptr;
    }


    void SceneManager::loadScene(const std::string& sceneName) {
        auto* assetManager = context->getService<IO::AssetManager>();
        if ( !assetManager ) {
            throw std::runtime_error("SceneManager: AssetManager not found");
        }

        auto it = sceneFilePaths.find(sceneName);
        if ( it == sceneFilePaths.end() ) {
            throw std::runtime_error("SceneManager: Unknown scene '" + sceneName + "'");
        }
        fs::path sceneFilePath = fs::path(assetsRoot) / it->second;
        auto sceneAsset = assetManager->load(sceneFilePath.string());
        auto jsonAsset = std::dynamic_pointer_cast<IO::JsonAsset>(sceneAsset);
        if ( !jsonAsset ) {
            throw std::runtime_error("SceneManager: Scene JSON failed to load correctly");
        }

        // Create a Scene instance
        auto scene = std::make_unique<Scene>(sceneName);
        scene->deserialize(jsonAsset->jsonDocument, *context, assetsRoot);

        // Cache for future reuse
        scenes[sceneName] = std::move(scene);
    }

    void SceneManager::performSceneSwitch() {
        if ( nextSceneQueued.empty() ) return;

        if ( currentScene ) {
            currentScene->shutdown(*context);
        }

        const std::string targetName = nextSceneQueued;
        nextSceneQueued.clear();
        switchingScene = false;

        auto targetSceneIt = scenes.find(targetName);
        // The scene is initialized and cached, bring it back up
        if ( targetSceneIt != scenes.end() && targetSceneIt->second ) {
            currentScene = targetSceneIt->second.get();
        }
        else { // scene is not cached, initialize it from io
            loadScene(targetName);
            currentScene = scenes[targetName].get();
        }

        if ( currentScene ) {
            currentScene->initialize(*context);
        }
    }

    void SceneManager::sceneListDeserialize() {
        if ( sceneListPath == "" ) {
            throw std::runtime_error("SceneManager: No set scene list path. Cannot load scenes.");
        }

        auto* assetManager = context->getService<IO::AssetManager>();
        if ( !assetManager ) {
            throw std::runtime_error("SceneManager: AssetManager not found in EngineContext");
        }

        // --- Load the scene list JSON file ---
        auto asset = assetManager->load(sceneListPath);
        auto jsonAsset = std::dynamic_pointer_cast<IO::JsonAsset>(asset);
        if ( !jsonAsset ) {
            throw std::runtime_error("SceneManager: Loaded scene list is not a JsonAsset");
        }

        const auto& doc = jsonAsset->jsonDocument;
        if ( !doc.IsObject() || !doc.HasMember("scenes") || !doc["scenes"].IsArray() ) {
            throw std::runtime_error("SceneManager: Scene list JSON missing valid 'scenes' array");
        }

        // --- Register scenes from JSON ---
        for ( const auto& entry : doc["scenes"].GetArray() ) {
            if ( !entry.HasMember("name") || !entry.HasMember("path") )
                continue;

            std::string name = entry["name"].GetString();
            std::string path = entry["path"].GetString();

            // deferred loading to optimize
            scenes[name] = nullptr;
            sceneFilePaths[name] = path;
        }

        if ( doc.HasMember("initialScene") && doc["initialScene"].IsString() ) {
            initialSceneName = doc["initialScene"].GetString();
        }
    }
}

