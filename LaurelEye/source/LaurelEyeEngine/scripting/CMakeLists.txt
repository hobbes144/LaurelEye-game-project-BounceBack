add_library(LaurelEyeScripting STATIC)

add_library(LaurelEye::Scripting ALIAS LaurelEyeScripting)

find_package(lua CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)

target_sources(LaurelEyeScripting PUBLIC
    "ScriptSystem.cpp"
    "ScriptTypeRegistry.cpp"
    "sol2/bindings/Sol2API_Animation.cpp"
    "sol2/bindings/Sol2API_Audio.cpp"
    "sol2/bindings/Sol2API_Camera.cpp"
    "sol2/bindings/Sol2API_ECS.cpp"
    "sol2/bindings/Sol2API_GameManager.cpp"
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
    "sol2/bindings/Sol2API_UIText.cpp"
    "sol2/bindings/Sol2API_Window.cpp"
    "sol2/Sol2API.cpp"
    "sol2/Sol2ScriptBroker.cpp"
    "sol2/Sol2ScriptInstance.cpp"
    "sol2/Sol2State.cpp"
)

target_precompile_headers(LaurelEyeScripting
    PRIVATE
        ${LAURELEYE_ROOT_DIR}/include/LaurelEyeEngine/pch.hpp
)

target_include_directories(LaurelEyeScripting
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(LaurelEyeScripting
    PUBLIC
        ${RAPIDJSON_INCLUDE_DIRS}
)

target_compile_definitions(LaurelEyeScripting
    PRIVATE
        LAURELEYE_SCRIPTING_BUILD
)

set_target_properties(LaurelEyeScripting PROPERTIES
    OUTPUT_NAME "LaurelEyeScripting"
    FOLDER "Engine"
)

target_link_libraries(LaurelEyeScripting
    PUBLIC
        lua
        sol2::sol2
)
