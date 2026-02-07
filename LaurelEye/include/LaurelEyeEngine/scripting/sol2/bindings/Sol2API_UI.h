/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   11-20-2025
/// @brief Handles binding the C++ ui library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/events/EventManager.h"
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace LaurelEye::Scripting {

    class Sol2API_UI {
    public:
        static void setup(sol::state& lua, EngineContext* ctx);
        static void shutdown(EngineContext* ctx);
    private:
        static void setupUIRenderComponent(sol::state& lua);
        static void setupUITransformComponent(sol::state& lua);
        //static void setupButtonCallbacks(sol::state& lua);
        //static void setupEventListener(sol::state& lua, EventManager* eventManager);

        static inline std::unordered_map<std::string, std::vector<sol::function>> buttonCallbacks;
        static inline uint32_t listenerID;
    };
} // namespace LaurelEye::Scripting
