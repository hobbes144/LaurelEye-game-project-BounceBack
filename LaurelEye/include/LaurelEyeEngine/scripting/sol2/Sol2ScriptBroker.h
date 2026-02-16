/// @file   Sol2ScriptBroker.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    02-08-2026
/// @brief Broker to manage script to script communication
#pragma once

#include "LaurelEyeEngine/scripting/IScriptBroker.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2Message.h"

#include <queue>
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <sol/string_view.hpp>

namespace LaurelEye::Scripting {

    class Sol2ScriptBroker : public ScriptBroker {
    public:
        void flushMessageQueue() override;
        void clearMessageQueue() override;

        void queueMessage(const SolMessage& msg);
        void queueBroadcast(const SolMessage& msg);

        void setupAPI(sol::state& lua);

    protected:
        std::queue<SolQueueMessage> messageQueueA;
        std::queue<SolQueueMessage> messageQueueB;

        bool currQueueA = true;

        void deliverMessage(const SolQueueMessage& msg);
    };
} // namespace LaurelEye::Scripting
