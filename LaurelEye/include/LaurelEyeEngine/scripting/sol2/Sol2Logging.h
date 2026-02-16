/// @file   Sol2Logging.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    02-09-2026
/// @brief Logging utility for Lua

#pragma once

#include "LaurelEyeEngine/logging/EngineLog.h"
#include <sol/state_view.hpp>
#include <sol/types.hpp>
extern "C"
{
#include <lauxlib.h>
#include <lua.h>
}

namespace LaurelEye::Scripting {

    struct LuaCallsite {
        std::string source; // script file / chunk
        std::string func;   // best-effort function name
        int line = -1;
    };

    // level_from_current=1 typically means "the Lua function that called this C++ function".
    inline LuaCallsite get_lua_callsite(lua_State* L, int level_from_current = 1) {
        lua_Debug ar{};
        LuaCallsite cs{};

        if ( lua_getstack(L, level_from_current, &ar) == 0 )
            return cs;

        // n = name, S = source, l = current line
        if ( lua_getinfo(L, "nSl", &ar) == 0 )
            return cs;

        cs.source = ar.short_src;
        cs.func = ar.name ? ar.name : "";
        cs.line = ar.currentline;

        return cs;
    }

} // namespace LaurelEye::Scripting

namespace LaurelEye::Log {
    inline void write_lua(Level lvl,
                          std::string_view subsystem,
                          std::string_view msg,
                          std::string_view lua_source,
                          int lua_line,
                          std::string_view lua_func = {}) {
        std::ostringstream out;

        if ( config().print_time ) {
            out << '[';
            append_time(out);
            out << "] ";
        }

        out << '[' << level_sv(lvl) << ']';

        if ( !subsystem.empty() )
            out << '[' << subsystem << ']';

        out << "[f=" << frame_counter().load(std::memory_order_relaxed) << ']';

        if ( config().print_thread )
            out << "[t=" << thread_id_u64() << ']';

        // Message
        out << ' ' << msg;

        // Lua location (instead of C++ source_location)
        if ( !lua_source.empty() && lua_line >= 0 ) {
            out << "  @ " << lua_source << ':' << lua_line;
            if ( !lua_func.empty() )
                out << " (" << lua_func << ')';
        }

        std::lock_guard<std::mutex> lock(sink_mutex());
        std::cerr << out.str() << '\n';
    }

    inline void luaLog(sol::this_state ts, const std::string& msg) {
#ifndef NDEBUG
        lua_State* L = ts.lua_state();
        auto cs = LaurelEye::Scripting::get_lua_callsite(L, 1);

        ::LaurelEye::Log::write_lua(::LaurelEye::Log::Level::Info,
                                    "Scripting",
                                    msg,
                                    cs.source,
                                    cs.line,
                                    cs.func);
#else
        LE_INFO("Scripting", msg);
#endif
    }

    inline void luaLogWarn(sol::this_state ts, const std::string& msg) {
#ifndef NDEBUG
        lua_State* L = ts.lua_state();
        auto cs = LaurelEye::Scripting::get_lua_callsite(L, 1);

        ::LaurelEye::Log::write_lua(::LaurelEye::Log::Level::Warn,
                                    "Scripting",
                                    msg,
                                    cs.source,
                                    cs.line,
                                    cs.func);
#else
        LE_WARN("Scripting", msg);
#endif
    }

    inline void luaLogError(sol::this_state ts, const std::string& msg) {
#ifndef NDEBUG
        lua_State* L = ts.lua_state();
        auto cs = LaurelEye::Scripting::get_lua_callsite(L, 1);

        ::LaurelEye::Log::write_lua(::LaurelEye::Log::Level::Error,
                                    "Scripting",
                                    msg,
                                    cs.source,
                                    cs.line,
                                    cs.func);
#else
        LE_ERROR("Scripting", msg);
#endif
    }

    inline void luaDebugLog(sol::this_state ts, const std::string& msg) {
#ifndef NDEBUG
        lua_State* L = ts.lua_state();
        auto cs = LaurelEye::Scripting::get_lua_callsite(L, 1);

        ::LaurelEye::Log::write_lua(::LaurelEye::Log::Level::Info,
                                    "Scripting",
                                    msg,
                                    cs.source,
                                    cs.line,
                                    cs.func);
#endif
    }

    inline void luaDebugLogWarn(sol::this_state ts, const std::string& msg) {
#ifndef NDEBUG
        lua_State* L = ts.lua_state();
        auto cs = LaurelEye::Scripting::get_lua_callsite(L, 1);

        ::LaurelEye::Log::write_lua(::LaurelEye::Log::Level::Warn,
                                    "Scripting",
                                    msg,
                                    cs.source,
                                    cs.line,
                                    cs.func);
#endif
    }

    inline void luaDebugLogError(sol::this_state ts, const std::string& msg) {
#ifndef NDEBUG
        lua_State* L = ts.lua_state();
        auto cs = LaurelEye::Scripting::get_lua_callsite(L, 1);

        ::LaurelEye::Log::write_lua(::LaurelEye::Log::Level::Error,
                                    "Scripting",
                                    msg,
                                    cs.source,
                                    cs.line,
                                    cs.func);
#endif
    }
} // namespace LaurelEye::Log
