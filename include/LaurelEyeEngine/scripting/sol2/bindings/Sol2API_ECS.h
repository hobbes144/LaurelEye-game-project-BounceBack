/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-29-2025
/// @brief Handles binding the C++ ecs library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_ECS {
    public:
        /// @brief Called by Sol2API in order to register all ecs bindings
        /// @param lua Lua state
        static void setup(sol::state& lua);

    private:
        static void setupEntityType(sol::state& lua);
        template <typename T>
        static sol::object safeFindComponent(Entity& e, sol::this_state s) {
            sol::state_view lua(s);
            if ( auto* c = e.findComponent<T>() )
                return sol::make_object(lua, c);
            return sol::make_object(lua, sol::nil);
        }
        static sol::object dynamicFindComponent(Entity& e, const std::string& type, sol::this_state s);
    };

} // namespace LaurelEye::Scripting
