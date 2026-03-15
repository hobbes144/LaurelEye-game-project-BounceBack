#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_GameManager.h"

namespace LaurelEye::Scripting {

    void Sol2API_GameManager::setup(sol::state& lua, EngineContext* ctx) {
        if ( !ctx ) return;

        GameManager* manager = ctx->getService<GameManager>();
        if ( !manager ) return;

        setupGameManager(lua, manager);
    }

    void Sol2API_GameManager::setupGameManager(sol::state& lua, GameManager* manager) {

        lua.new_usertype<GameManager>(
            "GameManager",
            sol::no_constructor,

            // Generators
            "addGeneratorDestroyed", &GameManager::addGeneratorDestroyed,
            "getGeneratorsDestroyed", &GameManager::getGeneratorsDestroyed,

            // Player health
            "setPlayerHealth", &GameManager::setPlayerHealth,
            "getPlayerHealth", &GameManager::getPlayerHealth,

            // Time
            "addTime", &GameManager::addTime,
            "setTimeCompleted", &GameManager::setTimeCompleted,
            "getTimeCompleted", &GameManager::getTimeCompleted,

            // Enemies
            "addEnemyKilled", &GameManager::addEnemyKilled,
            "getEnemiesKilled", &GameManager::getEnemiesKilled,

            // Combos
            "addCombo", &GameManager::addCombo,
            "resetCombo", &GameManager::resetCombo,
            "getCombos", &GameManager::getCombos,

            // Reset
            "resetStats", &GameManager::resetStats);

        // Global instance
        lua["GameManager"] = manager;
    }

} // namespace LaurelEye::Scripting
