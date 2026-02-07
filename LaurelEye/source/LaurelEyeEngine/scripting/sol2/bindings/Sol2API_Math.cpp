#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math.h"

namespace LaurelEye::Scripting {

    void Sol2API_Math::setup(sol::state& lua) {
        // Vector2
        setupVec2(lua);

        // Vector3
        setupVec3(lua);

        // Vector4
        setupVec4(lua);

        //Rect
        setupRect(lua);

        // Matrices
        setupMatrix4(lua);

        // Quaternion
        setupQuaternion(lua);

        // Transform
        setupTransform(lua);
    }

    void Sol2API_Math::setupVec2(sol::state& lua) {
        using Vector2 = VectorTemplated<float, 2>;
        lua.new_usertype<Vector2>(
            "Vector2",
            sol::no_constructor,
            // The default constructor not work for me, so we use factories instead
            // Factory functions
            "new", sol::factories([]() { return Vector2(); },                    // Default constructor
                                  [](float x, float y) { return Vector2(x, y); } // Two-arg constructor
                                  ),

            "x", sol::property([](Vector2& v) { return v[0]; }, [](Vector2& v, float value) { v[0] = value; }),
            "y", sol::property([](Vector2& v) { return v[1]; }, [](Vector2& v, float value) { v[1] = value; }));
    }

    void Sol2API_Math::setupVec3(sol::state& lua) {
        lua.new_usertype<LaurelEye::Vector3>("Vector3",
                                             // Constructors
                                             sol::constructors<
                                                 LaurelEye::Vector3(),
                                                 LaurelEye::Vector3(float, float, float),
                                                 LaurelEye::Vector3(float)>(),

                                             // Fields
                                             "x", sol::property([](LaurelEye::Vector3& v) { return v[0]; }, [](LaurelEye::Vector3& v, float val) { v[0] = val; }),
                                             "y", sol::property([](LaurelEye::Vector3& v) { return v[1]; }, [](LaurelEye::Vector3& v, float val) { v[1] = val; }),
                                             "z", sol::property([](LaurelEye::Vector3& v) { return v[2]; }, [](LaurelEye::Vector3& v, float val) { v[2] = val; }), // Methods
                                             "Dot", &LaurelEye::Vector3::dot,
                                             "Magnitude", &LaurelEye::Vector3::magnitude,
                                             "MagnitudeSquared", &LaurelEye::Vector3::magnitudSquared,
                                             "Normalized", &LaurelEye::Vector3::normalized,
                                             "Cross", &LaurelEye::Vector3::cross,
                                             "Reciprocal", &LaurelEye::Vector3::reciprocal,
                                             "Project", &LaurelEye::Vector3::project,
                                             "Abs", &LaurelEye::Vector3::abs,

                                             // Operator overloads
                                             sol::meta_function::addition, sol::overload([](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a + b; }, [](const LaurelEye::Vector3& a, float b) { return a + b; }),
                                             sol::meta_function::subtraction, sol::overload([](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a - b; }, [](const LaurelEye::Vector3& a, float b) { return a - b; }),
                                             sol::meta_function::multiplication, sol::overload([](const LaurelEye::Vector3& a, float b) { return a * b; }, [](float b, const LaurelEye::Vector3& a) { return a * b; }, [](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a * b; }),
                                             sol::meta_function::division, sol::overload([](const LaurelEye::Vector3& a, float b) { return a / b; }, [](const LaurelEye::Vector3& a, const LaurelEye::Vector3& b) { return a / b; }),
                                             sol::meta_function::equal_to, &LaurelEye::Vector3::operator==,
                                             sol::meta_function::unary_minus, [](const LaurelEye::Vector3& v) { return -v; });

        auto v3Table = lua["Vector3"];
        v3Table["Zero"] = LaurelEye::Vector3(0, 0, 0);
        v3Table["One"] = LaurelEye::Vector3(1, 1, 1);
        v3Table["Up"] = LaurelEye::Vector3(0, 1, 0);
        v3Table["Down"] = LaurelEye::Vector3(0, -1, 0);
        v3Table["Right"] = LaurelEye::Vector3(1, 0, 0);
        v3Table["Left"] = LaurelEye::Vector3(-1, 0, 0);
        v3Table["Forward"] = LaurelEye::Vector3(0, 0, 1);
        v3Table["Back"] = LaurelEye::Vector3(0, 0, -1);
    }

    void Sol2API_Math::setupVec4(sol::state& lua) {
        using Vec4 = LaurelEye::Vector4;

        lua.new_usertype<Vec4>("Vector4",
                               // Constructors
                               sol::constructors<
                                   Vec4(),
                                   Vec4(float),
                                   Vec4(float, float, float, float)>(),

                               // Fields
                               "x", sol::property([](Vec4& v) { return v[0]; }, [](Vec4& v, float val) { v[0] = val; }),
                               "y", sol::property([](Vec4& v) { return v[1]; }, [](Vec4& v, float val) { v[1] = val; }),
                               "z", sol::property([](Vec4& v) { return v[2]; }, [](Vec4& v, float val) { v[2] = val; }),
                               "w", sol::property([](Vec4& v) { return v[3]; }, [](Vec4& v, float val) { v[3] = val; }),

                               // Methods
                               "Dot", &Vec4::dot,
                               "Magnitude", &Vec4::magnitude,
                               "MagnitudeSquared", &Vec4::magnitudeSquared,
                               "Normalized", &Vec4::normalized,

                               // Operators
                               sol::meta_function::addition, sol::overload([](const Vec4& a, const Vec4& b) { return a + b; }),
                               sol::meta_function::subtraction, sol::overload([](const Vec4& a, const Vec4& b) { return a - b; }),
                               sol::meta_function::multiplication, sol::overload([](const Vec4& a, float b) { return a * b; }),
                               sol::meta_function::equal_to, &Vec4::operator==);
    }

    void Sol2API_Math::setupRect(sol::state& lua) {
        using Rect = LaurelEye::Rect;

        lua.new_usertype<Rect>(
            "Rect",

            // Constructors
            sol::constructors<
                Rect(),
                Rect(const LaurelEye::Vector2&, const LaurelEye::Vector2&)>(),

            // Fields
            "center", &Rect::center,
            "size", &Rect::size,

            // Edges
            "Left", &Rect::Left,
            "Top", &Rect::Top,
            "Right", &Rect::Right,
            "Bottom", &Rect::Bottom,

            // Corners
            "TopLeft", &Rect::TopLeft,
            "TopRight", &Rect::TopRight,
            "BottomLeft", &Rect::BottomLeft,
            "BottomRight", &Rect::BottomRight,

            // Center
            "SetCenter", &Rect::SetCenter,

            // Mutation
            "Translate", &Rect::Translate,
            "Inflate", &Rect::Inflate,

            // Queries
            "Contains", &Rect::Contains,
            "Intersects", &Rect::Intersects,
            "IsValid", &Rect::IsValid
        );
    }

} // namespace LaurelEye::Scripting

