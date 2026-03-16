#include "LaurelEyeEngine/scripting/ScriptTypeRegistry.h"
#include "LaurelEyeEngine/scene/SceneManager.h"
#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::Scripting {

    std::unordered_map<std::string, ScriptTypeParser>&
        ScriptTypeRegistry::getMap() {
        static std::unordered_map<std::string, ScriptTypeParser> parsers;
        return parsers;
    }

    void ScriptTypeRegistry::registerParser(
        const std::string& type,
        ScriptTypeParser parser) {
        getMap()[type] = parser;
    }

    std::optional<Scripting::ScriptValue>
        ScriptTypeRegistry::parse(
            const std::string& type,
            const rapidjson::Value& value) {
        auto& map = getMap();

        auto it = map.find(type);

        if ( it == map.end() ) {
            LE_DEBUG_WARN("ScriptTypeRegistry", "Type Not Registered with ScriptTypeRegistry");
            return std::nullopt;
        }

        return it->second(value);
    }

    ScriptValue ScriptTypeRegistry::ParseDynamicValue(const rapidjson::Value& v) {

        //----- Primitives -----

        if ( v.IsBool() )
            return ScriptValue(v.GetBool());

        if ( v.IsInt() )
            return ScriptValue(v.GetInt());

        if ( v.IsFloat() )
            return ScriptValue(v.GetFloat());

        if ( v.IsString() ) {

            std::string s = v.GetString();

            // Handle entity reference "@Name"
            if ( !s.empty() && s[0] == '@' ) {

                return ScriptValue(s);
            }

            return ScriptValue(s);
        }

        //----- Arrays -----

        if ( v.IsArray() ) {
            ScriptArray arr;

            for ( auto& e : v.GetArray() )
                arr.push_back(ParseDynamicValue(e));

            return ScriptValue(arr);
        }

        //----- Objects -----

        if ( v.IsObject() ) {

            // -------- Vector2 --------
            if ( v.HasMember("x") && v.HasMember("y") &&
                 v["x"].IsNumber() && v["y"].IsNumber() &&
                 v.MemberCount() == 2 ) {

                Vector2 vec;
                vec[0] = v["x"].GetFloat();
                vec[1] = v["y"].GetFloat();

                return ScriptValue(vec);
            }

            // -------- Vector3 --------
            if ( v.HasMember("x") && v.HasMember("y") && v.HasMember("z") &&
                 v["x"].IsNumber() && v["y"].IsNumber() && v["z"].IsNumber() &&
                 v.MemberCount() == 3 ) {

                Vector3 vec;
                vec.x = v["x"].GetFloat();
                vec.y = v["y"].GetFloat();
                vec.z = v["z"].GetFloat();

                return ScriptValue(vec);
            }

            // -------- Vector4 / Color --------
            if ( v.MemberCount() == 4 ) {

                Vector4 vec;

                if ( v.HasMember("x") ) {
                    vec[0] = v["x"].GetFloat();
                    vec[1] = v["y"].GetFloat();
                    vec[2] = v["z"].GetFloat();
                    vec[3] = v["w"].GetFloat();
                    return ScriptValue(vec);
                }

                if ( v.HasMember("r") ) {
                    vec[0] = v["r"].GetFloat();
                    vec[1] = v["g"].GetFloat();
                    vec[2] = v["b"].GetFloat();
                    vec[3] = v["a"].GetFloat();
                    return ScriptValue(vec);
                }
            }

            // -------- Generic Object --------

            ScriptObject obj;

            for ( auto itr = v.MemberBegin(); itr != v.MemberEnd(); ++itr ) {
                obj[itr->name.GetString()] =
                    ParseDynamicValue(itr->value);
            }

            return ScriptValue(obj);
        }

        return ScriptValue();
    }

}
