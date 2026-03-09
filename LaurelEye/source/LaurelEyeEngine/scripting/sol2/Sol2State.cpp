#include "LaurelEyeEngine/scripting/sol2/Sol2State.h"
#include "LaurelEyeEngine/logging/EngineLog.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"

#include "LaurelEyeEngine/io/Paths.h"

#include <string>
#include <filesystem>

namespace LaurelEye::Scripting {
    void Sol2State::initialize(EngineContext* ctx) {
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::debug, sol::lib::string, sol::lib::table);
        Sol2API::registerState(lua, ctx);

        // Release mode: use assets next to the EXE
        std::filesystem::path scriptDir = IO::Paths().assetsDir / "scripts";

        LE_ASSERT("scripting", std::filesystem::exists(scriptDir), "Unable to find scripts path!");

        std::string path = lua["package"]["path"];
        path += ";" + (scriptDir.string() + "/?.lua");
        for ( auto& entry : std::filesystem::directory_iterator(scriptDir) ) {
            if ( entry.is_directory() ) { path += ";" + (entry.path().string() + "/?.lua"); }
        }
        lua["package"]["path"] = path;
    }


    void Sol2State::shutdown(EngineContext* ctx) {
        Sol2API::shutdown(ctx);
        lua.collect_garbage();
    }

    std::unique_ptr<IScriptInstance> Sol2State::createInstance(const std::string& path, LaurelEye::Entity* owner) {
        return std::make_unique<Sol2ScriptInstance>(lua, path, owner);
    }
} // namespace LaurelEye::Scripting
