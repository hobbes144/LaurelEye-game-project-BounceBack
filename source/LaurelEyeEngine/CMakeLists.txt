# Engine library
# project (LaurelEyeEngine VERSION 0.0.1 DESCRIPTION "Team Laurel GAM55x Engine")

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiled.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/interfaces/Bullet/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/framerate/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/resources/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/device/glfw/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ecs/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platforms/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transform/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/graphics_components/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/surface/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io/importers/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scripting/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scripting/engines/sol2/*.cpp
)

file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/platforms/glfw/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/glfw/*.cpp
)

# Top-level CMakeLists.txt
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(FMODVendor)

find_package(Bullet CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)

add_library(LaurelEyeEngine SHARED ${ENGINE_SOURCES} ${PLATFORM_SOURCES})

le_add_fmod(LaurelEyeEngine USE_STUDIO)

# add_library(fmod SHARED IMPORTED GLOBAL)
# target_include_directories(LaurelEyeEngine PRIVATE ${FMOD_DIR})

# set(FMOD_DIR "${PROJECT_SOURCE_DIR}/external/fmod")

# if (WIN32)
#     # set_property(TARGET fmod PROPERTY
#     #              IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/win/core/lib/x64/fmod.dll")
#     # set_property(TARGET fmod PROPERTY
#     #              IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/win/core/lib/x64/fmod_vc.lib")
#     # set_property(TARGET fmod PROPERTY
#     #              INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/win/core/inc")
#
#     set_target_properties(fmod PROPERTIES IMPORTED_IMPLIB "${FMOD_DIR}/fmodlibs/fmod.lib")
#     set_target_properties(fmod PROPERTIES IMPORTED_IMPLIB "${FMOD_DIR}/fmodlibs/fmod_vc.lib")
# endif()



# add_library (LaurelEyeEngine
#   SHARED
#   "LaurelEyeEngine.cpp"
# )

target_precompile_headers(LaurelEyeEngine
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/LaurelEyeEngine/pch.hpp
#   PRIVATE
#     [["other_header.h"]]
#     <unordered_map>
)

# Public headers
target_include_directories(LaurelEyeEngine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(LaurelEyeEngine PROPERTIES PUBLIC_HEADER include/LaurelEyeEngine/LaurelEyeEngine.h)

target_compile_definitions(LaurelEyeEngine PUBLIC PLATFORM_GLFW)

if (CMAKE_VERSION VERSION_GREATER 3.20)
    set_property(TARGET LaurelEyeEngine PROPERTY CXX_STANDARD 23)
endif()

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

target_link_libraries(LaurelEyeEngine
    PUBLIC
        LinearMath
        BulletCollision
        BulletDynamics
        glfw
        glad::glad
        # fmod
        assimp::assimp
        lua
        sol2::sol2
)

target_include_directories(LaurelEyeEngine
    PUBLIC
        ${Stb_INCLUDE_DIR}            # provided by vcpkg
        ${RAPIDJSON_INCLUDE_DIRS}     # provided by vcpkg
)

add_custom_command(TARGET LaurelEyeEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        $<TARGET_FILE_DIR:LaurelEyeEngine>/assets)

# if (WIN32)
#     add_custom_command(TARGET LaurelEyeEngine POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${FMOD_DIR}/fmoddlls/fmod.dll"
#         $<TARGET_FILE_DIR:LaurelEyeEngine>)
#
#     add_custom_command(TARGET LaurelEyeEngine POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${FMOD_DIR}/fmoddlls/fmodL.dll"
#         $<TARGET_FILE_DIR:LaurelEyeEngine>)
#
#     add_custom_command(TARGET LaurelEyeEngine POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${FMOD_DIR}/fmoddlls/fmodstudio.dll"
#         $<TARGET_FILE_DIR:LaurelEyeEngine>)
#
#     add_custom_command(TARGET LaurelEyeEngine POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${FMOD_DIR}/fmoddlls/fmodstudioL.dll"
#         $<TARGET_FILE_DIR:LaurelEyeEngine>)
# endif()
