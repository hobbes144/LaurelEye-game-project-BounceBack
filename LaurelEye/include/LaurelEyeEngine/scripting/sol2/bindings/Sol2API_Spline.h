#pragma once
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Spline {
    public:
        /// @brief Called by Sol2API in order to register all math bindings
        /// @param lua Lua state
        static void setup(sol::state& lua);

    private:
        // Setting up individual math structures
        static void setupCatmullRomSpline(sol::state& lua);
        static void setupSpline(sol::state& lua);
    };

} // namespace LaurelEye::Scripting
