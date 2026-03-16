cmake_minimum_required(VERSION 3.8)

# Engine library
project (LaurelEyeEngine.Scripting VERSION 0.0.1 DESCRIPTION "Team Laurel GAM55x Engine Graphics lib")

find_package(lua CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)

add_library(LaurelEyeEngine.Scripting STATIC
    "ScriptSystem.cpp"
    "sol2/bindings/Sol2API_Animation.cpp"
    "sol2/bindings/Sol2API_Audio.cpp"
    "sol2/bindings/Sol2API_Camera.cpp"
    "sol2/bindings/Sol2API_ECS.cpp"
    "sol2/bindings/Sol2API_Input.cpp"
    "sol2/bindings/Sol2API_Lights.cpp"
    "sol2/bindings/Sol2API_Math2.cpp"
    "sol2/bindings/Sol2API_Math.cpp"
    "sol2/bindings/Sol2API_Particles.cpp"
    "sol2/bindings/Sol2API_Physics.cpp"
    "sol2/bindings/Sol2API_Renderable.cpp"
    "sol2/bindings/Sol2API_Scene.cpp"
    "sol2/bindings/Sol2API_Spline.cpp"
    "sol2/bindings/Sol2API_Transform.cpp"
    "sol2/bindings/Sol2API_UI.cpp"
    "sol2/Sol2API.cpp"
    "sol2/Sol2ScriptBroker.cpp"
    "sol2/Sol2ScriptInstance.cpp"
    "sol2/Sol2State.cpp"
)

le_add_fmod(LaurelEyeEngine.Scripting USE_STUDIO)

target_precompile_headers(LaurelEyeEngine.Scripting
    PRIVATE
        ${LE_ENGINE_PROJECT_ROOT}/include/LaurelEyeEngine/pch.hpp
)

# Public headers
target_include_directories(LaurelEyeEngine.Scripting
    PUBLIC
        ${LE_ENGINE_PROJECT_ROOT}/include
        ${LE_ENGINE_PROJECT_ROOT}/external
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(LaurelEyeEngine.Scripting PROPERTIES PUBLIC_HEADER ${LE_ENGINE_PROJECT_ROOT}/include/LaurelEyeEngine/LaurelEyeEngine.h)

target_compile_definitions(LaurelEyeEngine.Scripting PUBLIC PLATFORM_GLFW)

if (CMAKE_VERSION VERSION_GREATER 3.20)
    set_property(TARGET LaurelEyeEngine.Scripting PROPERTY CXX_STANDARD 23)
endif()

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

target_link_libraries(LaurelEyeEngine.Scripting
    PUBLIC
        lua
        sol2::sol2
)

target_compile_definitions(LaurelEyeEngine.Scripting
    PUBLIC PROJECT_ROOT_DIR="${LE_ENGINE_PROJECT_ROOT}"
)
