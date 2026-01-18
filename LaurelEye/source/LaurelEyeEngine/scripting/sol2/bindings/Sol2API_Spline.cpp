#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Spline.h"
#include <LaurelEyeEngine/math/CatmullRomSpline.h>

namespace LaurelEye::Scripting {

    void Sol2API_Spline::setup(sol::state& lua) {

        // Spline
        setupSpline(lua);

        // Catmull-Rom Spline
        setupCatmullRomSpline(lua);
    }

    

    void Sol2API_Spline::setupCatmullRomSpline(sol::state& lua) {
        lua.new_usertype<LaurelEye::CatmullRomSpline>(
            //"CatmullRomSpline", sol::no_constructor,
            "CatmullRomSpline", sol::constructors<LaurelEye::CatmullRomSpline()>(),
            sol::base_classes, sol::bases<LaurelEye::Spline>(),
            // same here, the default constructor not work for me
            "new", sol::factories([]() {
                return LaurelEye::CatmullRomSpline(); 
            }),
            // Properties
             "tension", sol::property([](LaurelEye::CatmullRomSpline& s) { return s.getTension(); },
                 [](LaurelEye::CatmullRomSpline& s, float t) { s.setTension(t); }),

            // Control point access
            "set_control_points", [](LaurelEye::CatmullRomSpline& s, sol::table points) {
            std::vector<LaurelEye::Vector2> vec;
            for (size_t i = 1; i <= points.size(); ++i) {  // Lua tables are 1-indexed
                vec.push_back(points[i]);
            }
            s.setControlPoints(vec); },

            "get_control_points", [](LaurelEye::CatmullRomSpline& s) { return s.getControlPoints(); },

            "at", [](LaurelEye::CatmullRomSpline& s, int i) -> LaurelEye::Vector2& { return s.at(i); },

            // Evaluation
            "evaluate", [](LaurelEye::CatmullRomSpline& s, float t) { return s.evaluate(t); },

            "derivative", [](LaurelEye::CatmullRomSpline& s, float t) { return s.derivative(t); },

            // Arc-length evaluation
            "evaluate_arc_length",
            [](LaurelEye::CatmullRomSpline& s, float u) { return s.evaluateArcLength(u); },

            "total_length", &LaurelEye::CatmullRomSpline::getTotalLength,

            // Serialization
            "save",
            [](LaurelEye::CatmullRomSpline& s, const std::string& filename) { s.toJSON(filename); },

            "load",
            [](LaurelEye::CatmullRomSpline& s, const std::string& filename) { s.fromJSON(filename); });

    }

    void Sol2API_Spline::setupSpline(sol::state& lua) {
        lua.new_usertype<LaurelEye::Spline>("Spline",
                                 sol::no_constructor,
                                 "evaluate_arc_length", &LaurelEye::Spline::evaluateArcLength,
                                 "total_length", &LaurelEye::Spline::getTotalLength,
                                 "set_control_points", &LaurelEye::Spline::setControlPoints,
                                 "get_control_points", [](LaurelEye::Spline& s) { return s.getControlPoints(); });
    }
} // namespace LaurelEye::Scripting
