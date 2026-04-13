#include "LaurelEyeEngine/scene/SceneManager.h"
#include "LaurelEyeEngine/graphics/renderpass/GBufferSkinnedPass.h"
#include "LaurelEyeEngine/graphics/renderpass/SkydomePass.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace LaurelEye {
    SceneManager::SceneManager(EngineContext& ctx, const EngineConfig& config) : context(&ctx), sceneListPath(""), currentScene(nullptr) {
        sceneListPath = config.initialSceneList;
        assetsRoot = config.assetRoot;
    }

    SceneManager::~SceneManager() {
        if ( deserializationThread.joinable() ) {
            deserializationRunning.store(false);
            deserializationThread.join();
        }

        currentScene = nullptr;
        scenes.clear();
        sceneFilePaths.clear();
        switchingScene = false;
    }

    void SceneManager::initialize() {
        deserializationRunning.store(true);
        deserializationThread = std::thread([this]() {
            try {
                sceneListDeserialize(); // synchronous loading, same as before
                assetIsLoaded.store(true, std::memory_order_release);
                std::cout << "[SceneManager] Scene list deserialization complete\n";
            }
            catch ( const std::exception& e ) {
                std::cerr << "[SceneManager] Error during scene deserialization: " << e.what() << "\n";
            }
            deserializationRunning.store(false);
        });

        auto* renderSystem = context->getService<Graphics::RenderSystem>();
        auto* assetManager = context->getService<IO::AssetManager>();
        renderSystem->setClearColor(0.208f, 0.222f, 0.236f);
        renderSystem->retrieveSkydomePass()->setTexture(Graphics::InvalidTexture);
        renderSystem->retrieveGBufferPass()->addSkydome(Graphics::InvalidTexture);
    }

    void SceneManager::update(float deltaTime) {
        if ( assetIsLoaded.exchange(false) ) {
            std::cout << "[SceneManager] Asset load flag detected, switching scene...\n";
            if ( !initialSceneName.empty() && hasScene(initialSceneName) )
                changeScene(initialSceneName);
            else if ( !scenes.empty() )
                changeScene(scenes.begin()->first);
        }

        if ( switchingScene && !nextSceneQueued.empty() ) {
            performSceneSwitch();
        }

        if ( pendingReload ) {
            pendingReload = false;
            if ( currentScene ) {
                currentScene->shutdown();
                currentScene->initialize();
                currentScene->registerScene();
            }
        }

        if ( currentScene ) {
            currentScene->update(deltaTime);
        }
    }

    void SceneManager::changeScene(const std::string& nextScene) {
        if ( !hasScene(nextScene) ) {
            throw std::runtime_error("SceneManager: Unknown scene '" + nextScene + "'");
        }
        // Prevent changing into the current scene
        if ( currentScene && nextScene == currentScene->getName() ) {
            reloadCurrentScene();
            return;
        }
        nextSceneQueued = nextScene;
        switchingScene = true;
    }

    void SceneManager::reloadCurrentScene() {
        if ( !currentScene ) return;

        pendingReload = true;
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

    Entity* SceneManager::instantiate(const std::string& prefabPath) {
        if ( currentScene ) {
            return currentScene->instantiate(prefabPath);
        }
        assert(false && "ERROR::SCENEMANAGER::INSTANTIATE::CURRENTSCENE_UNINITIALIZED");
        return nullptr;
    }

    void SceneManager::destroy(Entity* entity) {
        if ( currentScene && entity ) {
            currentScene->removeEntity(entity);
        }
    }

    void SceneManager::destroy(const std::string& entityName) {
        if ( currentScene ) {
            currentScene->removeEntity(entityName);
        }
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
        auto scene = std::make_unique<Scene>(sceneName, *context, jsonAsset, assetsRoot);
        scene->initialize(); // Initialize the scene once when its created

        // Cache for future reuse
        scenes[sceneName] = std::move(scene);
    }

    void SceneManager::performSceneSwitch() {
        if ( nextSceneQueued.empty() ) return;

        if ( currentScene ) {
            currentScene->deregisterScene();
        }

        const std::string targetName = nextSceneQueued;
        nextSceneQueued.clear();
        switchingScene = false;

        auto targetSceneIt = scenes.find(targetName);
        // The scene is initialized and cached, bring it back up
        if ( targetSceneIt != scenes.end() && targetSceneIt->second ) {
            currentScene = targetSceneIt->second.get();
            if ( currentScene->resetOnLoad() ) {
                currentScene->shutdown();
            }
            if ( currentScene->resetOnLoad() ) {
                currentScene->initialize();
            }
        }
        else { // This *shouldn't* happen, but just in case
            loadScene(targetName);
            currentScene = scenes[targetName].get();
        }

        if ( currentScene ) {
            currentScene->registerScene();
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

            fs::path sceneFilePath = fs::path(assetsRoot) / path;
            auto sceneAsset = assetManager->load(sceneFilePath.string());
            auto jsonAsset = std::dynamic_pointer_cast<IO::JsonAsset>(sceneAsset);
            if ( !jsonAsset ) {
                throw std::runtime_error("SceneManager: Scene JSON failed to load correctly for scene '" + name + "'");
            }
            auto scene = std::make_unique<Scene>(name, *context, jsonAsset, assetsRoot);
            scene->initialize();
            scenes[name] = std::move(scene);
            /*scenes[name] = nullptr;
            sceneFilePaths[name] = path;*/
        }

        if ( doc.HasMember("initialScene") && doc["initialScene"].IsString() ) {
            initialSceneName = doc["initialScene"].GetString();
        }
    }

#if !defined(NDEBUG)
    void SceneManager::injectSceneForTest(const std::string& name, std::unique_ptr<Scene> scene) {
        if ( !scene ) return;
        scenes[name] = std::move(scene);
        sceneFilePaths[name] = ""; // mark as manually inserted
    }

    void SceneManager::setCurrentScene(Scene* scene) {
        currentScene = scene;
    }
#endif
} // namespace LaurelEye
