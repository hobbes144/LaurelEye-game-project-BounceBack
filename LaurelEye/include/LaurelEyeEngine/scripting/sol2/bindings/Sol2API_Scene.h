/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-29-2025
/// @brief Handles binding the C++ scene/scene management library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/scene/SceneManager.h"
#include <sol/sol.hpp>


namespace LaurelEye::Scripting {
    class Sol2API_Scene {
    public:
        /// @brief Called by Sol2API in order to register all scene bindings
        /// @param lua Lua state
        static void setup(sol::state& lua, EngineContext* ctx);

    private:
        static void setupScene(sol::state& lua);
        static void setupSceneManager(sol::state& lua, SceneManager* manager);
    };

} // namespace LaurelEye::Scripting
