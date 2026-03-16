///
/// 
/// 

#pragma once

#include "LaurelEyeEngine/scripting/ScriptComponent.h"

#include <functional>
#include <optional>
#include <rapidjson/document.h>
#include <unordered_map>

namespace LaurelEye::Scripting {

    using ScriptTypeParser =
        std::function<std::optional<Scripting::ScriptValue>(
            const rapidjson::Value&)>;

    class ScriptTypeRegistry {
    public:
        static void registerParser(
            const std::string& type,
            ScriptTypeParser parser);

        static std::optional<ScriptValue> parse(
            const std::string& type,
            const rapidjson::Value& value);

        ScriptValue static ParseDynamicValue(const rapidjson::Value& v);

    private:
        static std::unordered_map<std::string, ScriptTypeParser>& getMap();
    };

}
