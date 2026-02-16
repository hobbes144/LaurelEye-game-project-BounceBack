/// @file   IScriptBroker.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    02-08-2026
/// @brief Broker to manage script to script communication
#pragma once

#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/scripting/IScriptInstance.h"

#include <unordered_map>

namespace LaurelEye::Scripting {

    /// @class IScriptBroker
    /// @brief This class takes care of script interactions
    ///
    /// Assumed that messaging functionality is implemented by the backend.
    ///
    class ScriptBroker {
    public:
        virtual ~ScriptBroker() = default;

        void registerScript(LaurelEye::Entity* entity, IScriptInstance* script) {
            scripts[entity] = script;
        }
        void deregisterScript(LaurelEye::Entity* entity) {
            scripts.erase(entity);
        }

        IScriptInstance* find(LaurelEye::Entity* entity) const {
            auto it = scripts.find(entity);
            return (it == scripts.end()) ? nullptr : it->second;
        }

        /// @brief Send all messages in queue.
        virtual void flushMessageQueue() = 0;
        virtual void clearMessageQueue() = 0;

    protected:
        std::unordered_map<LaurelEye::Entity*, IScriptInstance*> scripts;
    };
} // namespace LaurelEye::Scripting
