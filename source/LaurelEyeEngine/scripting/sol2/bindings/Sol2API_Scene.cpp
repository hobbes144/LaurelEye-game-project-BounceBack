#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Scene.h"
#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/ecs/Entity.h"

namespace LaurelEye::Scripting {

void Sol2API_Scene::setup(sol::state& lua, EngineContext* ctx) {
    if (!ctx) return;

    SceneManager* manager = ctx->getService<SceneManager>();
    if (!manager) return;

    setupScene(lua);
    setupSceneManager(lua, manager);
}

// ---- Bind Scene ----
void Sol2API_Scene::setupScene(sol::state& lua) {
    lua.new_usertype<Scene>(
        "Scene",
        sol::no_constructor,
        // Info
        "getName", &Scene::getName,
        "isPaused", &Scene::isPaused,

        // Entity management
        "addEntity", &Scene::addEntityFromRaw,
        "instantiate", &Scene::instantiate,
        "removeEntity", sol::overload(
            static_cast<void(Scene::*)(Entity*)>(&Scene::removeEntity),
            static_cast<void(Scene::*)(const std::string&)>(&Scene::removeEntity)
        ),
        "findEntityByName", &Scene::findEntityByName,
        "findEntityById", &Scene::findEntityById,
        "findEntitiesWithTag", &Scene::findEntitiesWithTag,
        "getEntities", &Scene::getEntityPointers
    );
}

// ---- Bind SceneManager ----
void Sol2API_Scene::setupSceneManager(sol::state& lua, SceneManager* manager) {
    // Expose SceneManager class
    lua.new_usertype<SceneManager>(
        "SceneManager",
        sol::no_constructor,
        "getCurrentScene", &SceneManager::getCurrentScene,
        "hasScene", &SceneManager::hasScene,
        "getScene", &SceneManager::getScene,
        "changeScene", &SceneManager::changeScene,
        "reloadCurrentScene", &SceneManager::reloadCurrentScene,
        "instantiate", &SceneManager::instantiate
    );

    // Expose the instance globally
    lua["SceneManager"] = manager;
}

} // namespace LaurelEye::Scripting
