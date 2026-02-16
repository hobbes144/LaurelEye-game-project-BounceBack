#pragma once

#include <sol/forward.hpp>
#include <sol/object.hpp>

namespace LaurelEye {
    class Entity;
}

namespace LaurelEye::Scripting {
    struct SolMessage {
        LaurelEye::Entity* from = nullptr;
        LaurelEye::Entity* to = nullptr;
        std::string topic;
        sol::object contents = sol::nil;
    };

    struct SolQueueMessage {
        LaurelEye::Entity* from = nullptr;
        LaurelEye::Entity* to = nullptr;
        std::string topic;
        sol::main_reference contents;
    };

    inline SolQueueMessage toQueueMessage(const SolMessage& msg) {
        return SolQueueMessage{msg.from, msg.to, msg.topic, sol::main_reference(msg.contents)};
    }

    inline SolMessage toMessage(const SolQueueMessage& msg) {
        return SolMessage{msg.from, msg.to, msg.topic, sol::object(msg.contents)};
    }

} // namespace LaurelEye::Scripting
