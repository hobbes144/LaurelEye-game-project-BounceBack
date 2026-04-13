# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.21)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(
        CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
endif()

project("GAM55X-Laurel"
    VERSION 0.1.0
    DESCRIPTION "LaurelEye Engine and Games"
    LANGUAGES CXX
)

# -------------------------------------------------
# Global settings
# -------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(GNUInstallDirs)

set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Enable warnings as errors globally
# if (MSVC)
#     add_compile_options(/W4 /WX)
# else()
#     add_compile_options(-Wall -Wextra -pedantic -Werror)
# endif()

# Options
option(LAURELEYE_BUILD_GAMES "Build games under /games" ON)
option(LAURELEYE_INSTALL_ENGINE "Install engine headers/library" OFF)

# -------------------------------------------------
# Module path
# -------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(LaurelEyeFmod)
include(LaurelEyeGame)

# Output directories
set(LE_OUTPUT_ROOT "${CMAKE_BINARY_DIR}/output")
set(LE_INSTALL_ROOT "${CMAKE_BINARY_DIR}/install")
set(LE_INTERMEDIATE_ROOT "${CMAKE_BINARY_DIR}/intermediate/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LE_INTERMEDIATE_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LE_INTERMEDIATE_ROOT}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LE_OUTPUT_ROOT}")
set(CMAKE_INSTALL_PREFIX "${LE_INSTALL_ROOT}")

set(LAURELEYE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LAURELEYE_ROOT_DIR}/cmake")

set(LAURELEYE_ENGINE_ASSETS_DIR "${LAURELEYE_ROOT_DIR}/assets")
set(LAURELEYE_ENGINE_INCLUDE_DIR "${LAURELEYE_ROOT_DIR}/include")
set(LAURELEYE_ENGINE_SOURCE_DIR "${LAURELEYE_ROOT_DIR}/source")
set(LAURELEYE_ENGINE_EXTERNAL_DIR "${LAURELEYE_ROOT_DIR}/external")

message("LAURELEYE_ENGINE_ROOT_DIR: ${LAURELEYE_ENGINE_ROOT_DIR}")
message("LAURELEYE_ENGINE_ASSETS_DIR: ${LAURELEYE_ENGINE_ASSETS_DIR}")
message("LAURELEYE_ENGINE_INCLUDE_DIR: ${LAURELEYE_ENGINE_INCLUDE_DIR}")
message("LAURELEYE_ENGINE_EXTERNAL_DIR: ${LAURELEYE_ENGINE_EXTERNAL_DIR}")

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
  "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
)

# -------------------------------------------------
# Dependencies from vcpkg
# -------------------------------------------------
find_package(assimp CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(Stb REQUIRED)

# -------------------------------------------------
# FMOD from external/
# -------------------------------------------------
laureleye_add_fmod(FMOD::FMOD)

# -------------------------------------------------
# Engine target
# -------------------------------------------------
add_library(LaurelEyeEngine STATIC)

add_library(LaurelEye::Engine ALIAS LaurelEyeEngine)

add_subdirectory(source/LaurelEyeEngine/math)
add_subdirectory(source/LaurelEyeEngine/graphics)
add_subdirectory(source/LaurelEyeEngine/scripting)
add_subdirectory(source/LaurelEyeEngine/physics)

# Using file GLOB to get all file names in case the file isn't added manually
# to the add_library call.
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${LAURELEYE_ENGINE_SOURCE_DIR}/*.cpp
)

# Replace these with your real file lists.
# Prefer explicit lists over GLOB for long-term maintenance.
target_sources(LaurelEyeEngine
    PRIVATE
    "source/LaurelEyeEngine/animation/AnimationManager.cpp"
    "source/LaurelEyeEngine/animation/AnimationSystem.cpp"
    "source/LaurelEyeEngine/animation/Component/SkeletalAnimationComponent.cpp"
    "source/LaurelEyeEngine/audio/AudioComponent.cpp"
    "source/LaurelEyeEngine/audio/AudioSystem.cpp"
    "source/LaurelEyeEngine/audio/FModAudioManager.cpp"
    "source/LaurelEyeEngine/core/Engine.cpp"
    "source/LaurelEyeEngine/core/ResourceCoordinator.cpp"
    "source/LaurelEyeEngine/core/SystemCoordinator.cpp"
    "source/LaurelEyeEngine/debugDraw/DebugDrawSystem.cpp"
    "source/LaurelEyeEngine/ecs/EntityFactory.cpp"
    "source/LaurelEyeEngine/input/GLFWInput.cpp"
    "source/LaurelEyeEngine/input/InputManager.cpp"
    "source/LaurelEyeEngine/io/AssetManager.cpp"
    "source/LaurelEyeEngine/io/FileSystem.cpp"
    "source/LaurelEyeEngine/io/importers/AudioAssetImporter.cpp"
    "source/LaurelEyeEngine/io/importers/ImageImporter.cpp"
    "source/LaurelEyeEngine/io/importers/JsonImporter.cpp"
    "source/LaurelEyeEngine/io/importers/MeshImporter.cpp"
    "source/LaurelEyeEngine/io/importers/ShaderImporter.cpp"
    "source/LaurelEyeEngine/io/importers/TextImporter.cpp"
    "source/LaurelEyeEngine/io/Paths.cpp"
    "source/LaurelEyeEngine/memory/MemoryManager.cpp"
    "source/LaurelEyeEngine/particles/ParticleEmitterComponent.cpp"
    "source/LaurelEyeEngine/particles/ParticleSystem.cpp"
    "source/LaurelEyeEngine/platforms/glfw/GlfwPlatform.cpp"
    "source/LaurelEyeEngine/scene/Scene.cpp"
    "source/LaurelEyeEngine/scene/SceneManager.cpp"
    "source/LaurelEyeEngine/transform/TransformSystem.cpp"
    "source/LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.cpp"
    "source/LaurelEyeEngine/UI/UIComponents/UIRenderComponent.cpp"
    "source/LaurelEyeEngine/UI/UIComponents/UITextComponent.cpp"
    "source/LaurelEyeEngine/UI/UIComponents/UITransformComponent.cpp"
    "source/LaurelEyeEngine/UI/UIInteractionSystem.cpp"
    "source/LaurelEyeEngine/UI/UILayoutSystem.cpp"
    "source/LaurelEyeEngine/window/glfw/LGlfwWindow.cpp"
    "source/LaurelEyeEngine/window/WindowManager.cpp"
    # ${ENGINE_SOURCES}
    )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Actions specific to the Debug build type
    message(STATUS "Debug mode detected. Enabling debug-specific actions.")
    add_compile_definitions(SOL_ALL_SAFETIES_ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Actions specific to the Release build type
    message(STATUS "Release mode detected.")
endif()

target_precompile_headers(LaurelEyeEngine
    PRIVATE
        ${LAURELEYE_ENGINE_INCLUDE_DIR}/LaurelEyeEngine/pch.hpp
)

target_compile_definitions(LaurelEyeEngine
    PUBLIC PROJECT_ROOT_DIR="${PROJECT_SOURCE_DIR}"
)

set_target_properties(LaurelEyeEngine
    PROPERTIES
        PUBLIC_HEADER include/LaurelEyeEngine/LaurelEyeEngine.h)

target_include_directories(LaurelEyeEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(LaurelEyeEngine
    PUBLIC
        LaurelEyeMath
        LaurelEyeGraphics
        LaurelEyeScripting
        LaurelEyePhysics
        assimp::assimp
        FMOD::FMOD
)

target_compile_features(LaurelEyeEngine PUBLIC cxx_std_23)

# RapidJSON / stb are often header-only in practice.
target_include_directories(LaurelEyeEngine
    PUBLIC
        ${Stb_INCLUDE_DIR}
        ${RAPIDJSON_INCLUDE_DIRS}
)

# Organize in IDEs
set_target_properties(LaurelEyeEngine PROPERTIES
    FOLDER "Engine"
    OUTPUT_NAME "LaurelEyeEngine"
)

# -------------------------------------------------
# Install engine (optional SDK-style install)
# -------------------------------------------------
if(LAURELEYE_INSTALL_ENGINE)
    install(TARGETS LaurelEyeEngine
        EXPORT LaurelEyeTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT LaurelEyeTargets
        FILE LaurelEyeTargets.cmake
        NAMESPACE LaurelEye::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LaurelEye
    )
endif()

# -------------------------------------------------
# Games
# -------------------------------------------------
set(
    LAURELEYE_OUTPUT_DIR
    "${CMAKE_BINARY_DIR}/output/games/$<CONFIG>"
    CACHE PATH "Game output directory")
if(LAURELEYE_BUILD_GAMES)
    add_subdirectory(games)
endif()

# -------------------------------------------------
# Packaging
# -------------------------------------------------
set(CPACK_PACKAGE_NAME "LaurelEye")
set(CPACK_PACKAGE_VENDOR "LaurelEye")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

# Produces zip/tgz packages easily.
# Later you can add NSIS/WIX on Windows if you want an installer.
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

option(BUILD_TESTS "Build Engine Tests" OFF)
add_subdirectory("tests/LaurelEyeEngineUnitTests")

option(BUILD_DOCS "Build API documentation" OFF)
add_subdirectory("doc")
