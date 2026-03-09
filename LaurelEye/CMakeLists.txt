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
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(lua CONFIG REQUIRED)
# find_package(unofficial-lua CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)

# Normalize Bullet targets across package variants
set(LAURELEYE_BULLET_LIBS "")
if(TARGET BulletDynamics AND TARGET BulletCollision AND TARGET LinearMath)
    list(APPEND LAURELEYE_BULLET_LIBS BulletDynamics BulletCollision LinearMath)
elseif(TARGET Bullet::BulletDynamics AND TARGET Bullet::BulletCollision AND TARGET Bullet::LinearMath)
    list(APPEND LAURELEYE_BULLET_LIBS Bullet::BulletDynamics Bullet::BulletCollision Bullet::LinearMath)
else()
    message(FATAL_ERROR "Could not determine Bullet targets exposed by vcpkg.")
endif()

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
    "source/LaurelEyeEngine/audio/AudioAsset.cpp"
    "source/LaurelEyeEngine/audio/AudioSystem.cpp"
    "source/LaurelEyeEngine/audio/FModAudioManager.cpp"
    "source/LaurelEyeEngine/audio/SpeakerComponent.cpp"
    "source/LaurelEyeEngine/core/Engine.cpp"
    "source/LaurelEyeEngine/core/ResourceCoordinator.cpp"
    "source/LaurelEyeEngine/core/SystemCoordinator.cpp"
    "source/LaurelEyeEngine/debugDraw/DebugDrawSystem.cpp"
    "source/LaurelEyeEngine/ecs/EntityFactory.cpp"
    "source/LaurelEyeEngine/graphics/device/glfw/LGLDataBufferFactory.cpp"
    "source/LaurelEyeEngine/graphics/device/glfw/LGLFramebufferFactory.cpp"
    "source/LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.cpp"
    "source/LaurelEyeEngine/graphics/device/glfw/LGLRenderState.cpp"
    "source/LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.cpp"
    "source/LaurelEyeEngine/graphics/device/glfw/LGLVertexArrayFactory.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/CameraComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/IRenderComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/LightComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/PointLightComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/Renderable2DComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/UIButtonComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/UIComponent.cpp"
    "source/LaurelEyeEngine/graphics/graphics_components/UISliderComponent.cpp"
    "source/LaurelEyeEngine/graphics/MeshManager.cpp"
    "source/LaurelEyeEngine/graphics/MeshManagerPrimitives.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/DebugDrawRenderPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/DeferredRenderPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/GBufferPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/GBufferSkinnedPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/IRenderPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/LocalLightsPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/ParticleRenderPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/PointShadowSkinnedPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/ShadowPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/ShadowSkinnedPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/SingleBufferedDataPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/SinglePass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/SinglePassShadow.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/SkydomePass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/LocalLightsToonPass.cpp"
    "source/LaurelEyeEngine/graphics/renderpass/UIPass.cpp"
    "source/LaurelEyeEngine/graphics/RenderStateSaver.cpp"
    "source/LaurelEyeEngine/graphics/RenderSystem.cpp"
    "source/LaurelEyeEngine/graphics/resources/GeometryBuffer.cpp"
    "source/LaurelEyeEngine/graphics/resources/Material.cpp"
    "source/LaurelEyeEngine/graphics/resources/Mesh.cpp"
    "source/LaurelEyeEngine/graphics/resources/RenderResources.cpp"
    "source/LaurelEyeEngine/graphics/resources/Shader.cpp"
    "source/LaurelEyeEngine/graphics/ShaderManager.cpp"
    "source/LaurelEyeEngine/graphics/ShadowManager.cpp"
    "source/LaurelEyeEngine/graphics/SkeletonManager.cpp"
    "source/LaurelEyeEngine/graphics/surface/glfw/LGlfwOpenGLWindowSurface.cpp"
    "source/LaurelEyeEngine/input/GLFWInput.cpp"
    "source/LaurelEyeEngine/input/InputManager.cpp"
    "source/LaurelEyeEngine/io/AssetManager.cpp"
    "source/LaurelEyeEngine/io/FileSystem.cpp"
    "source/LaurelEyeEngine/io/importers/ImageImporter.cpp"
    "source/LaurelEyeEngine/io/importers/JsonImporter.cpp"
    "source/LaurelEyeEngine/io/importers/MeshImporter.cpp"
    "source/LaurelEyeEngine/io/importers/ShaderImporter.cpp"
    "source/LaurelEyeEngine/io/importers/TextImporter.cpp"
    "source/LaurelEyeEngine/memory/MemoryManager.cpp"
    "source/LaurelEyeEngine/particles/ParticleEmitterComponent.cpp"
    "source/LaurelEyeEngine/particles/ParticleSystem.cpp"
    "source/LaurelEyeEngine/physics/CollisionManager.cpp"
    "source/LaurelEyeEngine/physics/interfaces/Bullet/BulletRigidBody.cpp"
    "source/LaurelEyeEngine/physics/interfaces/Bullet/BulletGhostBody.cpp"
    "source/LaurelEyeEngine/physics/interfaces/Bullet/BulletShape.cpp"
    "source/LaurelEyeEngine/physics/interfaces/Bullet/BulletWorld.cpp"
    "source/LaurelEyeEngine/physics/RigidBodyComponent.cpp"
    "source/LaurelEyeEngine/physics/GhostBodyComponent.cpp"
    "source/LaurelEyeEngine/physics/PhysicsQueryAPI.cpp"
    "source/LaurelEyeEngine/physics/PhysicsSystem.cpp"
    "source/LaurelEyeEngine/platforms/glfw/GlfwPlatform.cpp"
    "source/LaurelEyeEngine/scene/Scene.cpp"
    "source/LaurelEyeEngine/scene/SceneManager.cpp"
    "source/LaurelEyeEngine/scripting/ScriptSystem.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Animation.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Audio.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Camera.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_ECS.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Input.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Lights.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math2.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Particles.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Physics.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Renderable.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Scene.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Spline.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Transform.cpp"
    "source/LaurelEyeEngine/scripting/sol2/bindings/Sol2API_UI.cpp"
    "source/LaurelEyeEngine/scripting/sol2/Sol2API.cpp"
    "source/LaurelEyeEngine/scripting/sol2/Sol2ScriptBroker.cpp"
    "source/LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.cpp"
    "source/LaurelEyeEngine/scripting/sol2/Sol2State.cpp"
    "source/LaurelEyeEngine/transform/TransformSystem.cpp"
    "source/LaurelEyeEngine/UI/UIComponents/UIRenderComponent.cpp"
    "source/LaurelEyeEngine/UI/UIComponents/UITransformComponent.cpp"
    "source/LaurelEyeEngine/UI/UILayoutSystem.cpp"
    "source/LaurelEyeEngine/window/glfw/LGlfwWindow.cpp"
    "source/LaurelEyeEngine/window/WindowManager.cpp"
    ${ENGINE_SOURCES}
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

target_compile_definitions(LaurelEyeEngine PUBLIC PLATFORM_GLFW)

target_compile_definitions(LaurelEyeEngine
    PUBLIC PROJECT_ROOT_DIR="${PROJECT_SOURCE_DIR}"
)

set_target_properties(LaurelEyeEngine PROPERTIES PUBLIC_HEADER include/LaurelEyeEngine/LaurelEyeEngine.h)

target_include_directories(LaurelEyeEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(LaurelEyeEngine
    PUBLIC
        LaurelEyeMath
        glad::glad
        glfw
        assimp::assimp
        lua
        sol2::sol2
        ${LAURELEYE_BULLET_LIBS}
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
