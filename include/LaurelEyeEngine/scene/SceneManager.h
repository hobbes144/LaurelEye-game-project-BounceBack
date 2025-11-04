/// @file   SceneManager.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-22-2025
/// @brief  Manages scenes accessible in the current running game isntance.
/// Also notes the current scene, and the capability of modifying which scene that is.
///
/// To note - the "thing" that is actively playing on screen is made up of our list of entities,
/// but more importantly the data in them registered to the systems. So a scene in action
/// is really just the correct entities registered into all our systems at the right time.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <rapidjson/document.h>

#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/core/Config.h"

namespace LaurelEye {
    class SceneManager {
    public:
        SceneManager(EngineContext& ctx, const EngineConfig& config);
        ~SceneManager();

        /// @brief Deserializes the scene list and assigns a starting scene
        void initialize();

        /// @brief Handles calling scene's update (which is mainly used for syncing it's entity list),
        /// and for itself syncing the current scene
        /// @param deltaTime If needed
        void update(float deltaTime);

        /// @brief Moves to the given scene using the scene's name. Should match what is setup in data
        /// @param nextScene The name of the scene to move to
        void changeScene(const std::string& nextScene);

        //void unloadCurrentScene(); tbd

        /// @brief Restarts the "currentScene". I.e., calls shutdown and start on the scene
        void reloadCurrentScene();

        Scene* getCurrentScene() const { return currentScene; }
        bool hasScene(const std::string& name) const;
        Scene* getScene(const std::string& name);

        /// @brief Called from script or other systems. Instantiates prefab into the current scene
        /// @param prefabPath 
        Entity* instantiate(const std::string& prefabPath);

        // --- Test and manual setup utilities ---
#if !defined(NDEBUG)
        /// @brief Allows tests or debug tools to directly insert a scene into the manager
        void injectSceneForTest(const std::string& name, std::unique_ptr<Scene> scene);

        /// @brief Sets the current scene manually, bypassing normal changeScene() flow.
        /// Use only for testing or debug initialization.
        void setCurrentScene(Scene* scene);
#endif
    private:
        // Context for other engine services
        EngineContext* context = nullptr;

        // Path to the scene list for use when deserializing
        std::string sceneListPath;
        // Name of the scene to start with, either assigned in JSON or the first scene in the list
        std::string initialSceneName;
        // The root folder to load asset files
        std::string assetsRoot;

        // Maps a scene's name to it's held instance
        std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
        // Map of scene name to file path (from JSON) for deferred loading
        std::unordered_map<std::string, std::string> sceneFilePaths;

        // The scene currently active for all systems. Raw ptr jsut looking at the scenes map
        Scene* currentScene;
        // The name of the scene to transition to when the frame ends
        std::string nextSceneQueued;
        // Whether we are switching scenes or not
        bool switchingScene = false;
        bool pendingReload = false;

        /// @brief Called between frames. Searches the sceneFilePaths for the given name
        /// and calls AssetManager to load the scene json
        /// @param sceneName The name of the scene to load
        void loadScene(const std::string& sceneName);
        /// @brief Called between frames. Takes the nextSceneQueued name and loads it
        void performSceneSwitch();

        /// @brief Using the stored sceneListPath, deserialize the scene list json
        /// in order to populate the sceneFilePaths for deferred loading
        void sceneListDeserialize();
    };
} // namespace LaurelEye
