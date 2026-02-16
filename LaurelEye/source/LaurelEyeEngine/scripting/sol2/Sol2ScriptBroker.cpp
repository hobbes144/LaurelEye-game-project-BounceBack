#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptBroker.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2Message.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"

#include "LaurelEyeEngine/logging/EngineLog.h"
#include <sol/forward.hpp>

namespace LaurelEye::Scripting {
    void Sol2ScriptBroker::flushMessageQueue() {
        auto& messageQueue = currQueueA ? messageQueueA : messageQueueB;
        currQueueA = !currQueueA;
        while ( !messageQueue.empty() ) {
            deliverMessage(messageQueue.front());
            messageQueue.pop();
        }
    }

    void Sol2ScriptBroker::clearMessageQueue() {
        auto& messageQueue = currQueueA ? messageQueueA : messageQueueB;
        currQueueA = !currQueueA;
        while ( !messageQueue.empty() ) {
            messageQueue.pop();
        }
    }

    void Sol2ScriptBroker::queueMessage(const SolMessage& msg) {
        auto& messageQueue = currQueueA ? messageQueueA : messageQueueB;
        messageQueue.push(toQueueMessage(msg));
    }

    void Sol2ScriptBroker::queueBroadcast(const SolMessage& msg) {
        auto& messageQueue = currQueueA ? messageQueueA : messageQueueB;
        auto msgContents = sol::main_reference(msg.contents);
        for ( auto& [entity, inst] : scripts ) {
            messageQueue.push({msg.from,
                               entity,
                               msg.topic,
                               msgContents});
        }
    }

    void Sol2ScriptBroker::deliverMessage(const SolQueueMessage& msg) {
        auto instance = static_cast<Sol2ScriptInstance*>(find(msg.to));
        LE_WARN_IF("Scripting", !instance, "message target not found for message: " << msg.topic);
        if ( instance ) instance->onMessage(toMessage(msg));
    }

    void Sol2ScriptBroker::setupAPI(sol::state& lua) {
        // Messaging
        lua.new_usertype<SolMessage>(
            "Message",
            sol::constructors<SolMessage()>(),
            // Info
            "from", &SolMessage::from,
            "to", &SolMessage::to,
            "topic", &SolMessage::topic,
            "contents", &SolMessage::contents);

        sol::table Script = lua["Script"].get_or_create<sol::table>();

        Script.set_function("send", [this](const SolMessage& msg) { this->queueMessage(msg); });
        Script.set_function("broadcast", [this](const SolMessage& msg) { this->queueBroadcast(msg); });
    }

} // namespace LaurelEye::Scripting
