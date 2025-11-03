#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"
#include "LaurelEyeEngine/io/FileSystem.h"

namespace LaurelEye::Scripting {

    Sol2ScriptInstance::Sol2ScriptInstance(sol::state& luaState, const std::string& scriptPath, LaurelEye::Entity* owner_)
        : lua(luaState), env(luaState, sol::create, luaState.globals()), owner(owner_) {

        Sol2API::registerEnvironment(env, owner);
        // TODO - somehow let AssetManager do this?
        auto fullPath = IO::resolve(scriptPath);
        try {
            lua.script_file(fullPath.string(), env);
        }
        catch ( const sol::error& e ) {
            std::cerr << "[Lua] script_file error: " << e.what() << std::endl;
        }

        startFunc = env["onStart"];
        updateFunc = env["onUpdate"];
        shutdownFunc = env["onShutdown"];
    }

    Sol2ScriptInstance::~Sol2ScriptInstance() {
        invalidate();
    }

    void Sol2ScriptInstance::onStart() {
        if ( startFunc.valid() ) {
            auto result = startFunc();
            if ( !result.valid() ) {
                sol::error err = result;
                std::cerr << "[Lua onStart Error] " << err.what() << std::endl;
            }
        }
    }

    void Sol2ScriptInstance::onUpdate(float dt) {
        if ( updateFunc.valid() ) {
            auto result = updateFunc(dt);
            if ( !result.valid() ) {
                sol::error err = result;
                std::cerr << "[Lua onUpdate Error] " << err.what() << std::endl;
            }
        }

    }

    void Sol2ScriptInstance::onShutdown() {
        if ( shutdownFunc.valid() ) {
            auto result = shutdownFunc();
            if ( !result.valid() ) {
                sol::error err = result;
                std::cerr << "[Lua onShutdown Error] " << err.what() << std::endl;
            }
        }
        invalidate();
    }
    void Sol2ScriptInstance::invalidate() {
        startFunc = sol::nil;
        updateFunc = sol::nil;
        shutdownFunc = sol::nil;
        env = sol::nil;
        owner = nullptr;
    }
} // namespace LaurelEye::Scripting
