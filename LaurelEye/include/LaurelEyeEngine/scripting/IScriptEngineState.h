/// @file   IScriptEngineState.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-7-2025
/// @brief  The external state of a scripting engine, i.e.
/// a sol::state for sol2/lua.
/// Handles initializing and shutting down the state,
/// as well as abstractly creating new script instances,
/// registering them to their world state
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <memory>
#include <string>
#include <LaurelEyeEngine/ecs/Entity.h>
#include "LaurelEyeEngine/core/EngineContext.h"

namespace LaurelEye::Scripting {
    class ScriptComponent;
    class IScriptInstance;
    class IScriptEngineState {
    public:
        virtual ~IScriptEngineState() = default;

        /// @brief Initialize the scripting environment (open libraries, register API, etc.)
        virtual void initialize(EngineContext* context) = 0;

        /// @brief Shutdown and cleanup scripting resources
        virtual void shutdown(EngineContext* ctx) = 0;

        /// @brief Create a new script instance bound to this engine state
        /// @param path The path to the script file
        virtual std::unique_ptr<IScriptInstance> createInstance(const std::string& path, LaurelEye::Entity* owner, LaurelEye::Scripting::ScriptComponent* sc) = 0;
    };

} // namespace LaurelEye::Scripting
