# Engine library
# project (LaurelEyeEngine VERSION 0.0.1 DESCRIPTION "Team Laurel GAM55x Engine")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Actions specific to the Debug build type
    message(STATUS "Debug mode detected. Enabling debug-specific actions.")
    add_compile_definitions(SOL_ALL_SAFETIES_ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Actions specific to the Release build type
    message(STATUS "Release mode detected.")
endif()

# Using file GLOB to get all file names in case the file isn't added manually
# to the add_library call.
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
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

add_library(LaurelEyeEngine SHARED
    "animation/AnimationManager.cpp"
    "animation/AnimationSystem.cpp"
    "animation/Component/SkeletalAnimationComponent.cpp"
    "audio/AudioAsset.cpp"
    "audio/AudioSystem.cpp"
    "audio/FModAudioManager.cpp"
    "audio/SpeakerComponent.cpp"
    "core/Engine.cpp"
    "core/ResourceCoordinator.cpp"
    "core/SystemCoordinator.cpp"
    "debugDraw/DebugDrawSystem.cpp"
    "ecs/EntityFactory.cpp"
    "graphics/device/glfw/LGLDataBufferFactory.cpp"
    "graphics/device/glfw/LGLFramebufferFactory.cpp"
    "graphics/device/glfw/LGLRenderDevice.cpp"
    "graphics/device/glfw/LGLRenderState.cpp"
    "graphics/device/glfw/LGLTextureFactory.cpp"
    "graphics/device/glfw/LGLVertexArrayFactory.cpp"
    "graphics/graphics_components/AmbientLightComponent.cpp"
    "graphics/graphics_components/CameraComponent.cpp"
    "graphics/graphics_components/DirectionalLightComponent.cpp"
    "graphics/graphics_components/IRenderComponent.cpp"
    "graphics/graphics_components/LightComponent.cpp"
    "graphics/graphics_components/PointLightComponent.cpp"
    "graphics/graphics_components/Renderable2DComponent.cpp"
    "graphics/graphics_components/Renderable3DComponent.cpp"
    "graphics/graphics_components/UIButtonComponent.cpp"
    "graphics/graphics_components/UIComponent.cpp"
    "graphics/graphics_components/UISliderComponent.cpp"
    "graphics/MeshManager.cpp"
    "graphics/MeshManagerPrimitives.cpp"
    "graphics/renderpass/DebugDrawRenderPass.cpp"
    "graphics/renderpass/DeferredRenderPass.cpp"
    "graphics/renderpass/GBufferPass.cpp"
    "graphics/renderpass/GBufferSkinnedPass.cpp"
    "graphics/renderpass/IRenderPass.cpp"
    "graphics/renderpass/LocalLightsPass.cpp"
    "graphics/renderpass/ParticleRenderPass.cpp"
    "graphics/renderpass/PointShadowSkinnedPass.cpp"
    "graphics/renderpass/ShadowPass.cpp"
    "graphics/renderpass/ShadowSkinnedPass.cpp"
    "graphics/renderpass/SingleBufferedDataPass.cpp"
    "graphics/renderpass/SinglePass.cpp"
    "graphics/renderpass/SinglePassShadow.cpp"
    "graphics/renderpass/SkydomePass.cpp"
    "graphics/renderpass/UIPass.cpp"
    "graphics/RenderStateSaver.cpp"
    "graphics/RenderSystem.cpp"
    "graphics/resources/GeometryBuffer.cpp"
    "graphics/resources/Material.cpp"
    "graphics/resources/Mesh.cpp"
    "graphics/resources/RenderResources.cpp"
    "graphics/resources/Shader.cpp"
    "graphics/ShaderManager.cpp"
    "graphics/ShadowManager.cpp"
    "graphics/SkeletonManager.cpp"
    "graphics/surface/glfw/LGlfwOpenGLWindowSurface.cpp"
    "input/GLFWInput.cpp"
    "input/InputManager.cpp"
    "io/AssetManager.cpp"
    "io/FileSystem.cpp"
    "io/importers/ImageImporter.cpp"
    "io/importers/JsonImporter.cpp"
    "io/importers/MeshImporter.cpp"
    "io/importers/ShaderImporter.cpp"
    "io/importers/TextImporter.cpp"
    "math/ArcLengthTable.cpp"
    "math/CatmullRomSpline.cpp"
    "math/EngineMath.cpp"
    "math/Matrix4.cpp"
    "math/Quaternion.cpp"
    "math/Spline.cpp"
    "math/Transform.cpp"
    "math/Vector3.cpp"
    "math/VectorTemplated.cpp"
    "memory/MemoryManager.cpp"
    "particles/ParticleEmitterComponent.cpp"
    "particles/ParticleSystem.cpp"
    "physics/CollisionManager.cpp"
    "physics/interfaces/Bullet/BulletRigidBody.cpp"
    "physics/interfaces/Bullet/BulletGhostBody.cpp"
    "physics/interfaces/Bullet/BulletShape.cpp"
    "physics/interfaces/Bullet/BulletWorld.cpp"
    "physics/RigidBodyComponent.cpp"
    "physics/GhostBodyComponent.cpp"
    "physics/PhysicsQueryAPI.cpp"
    "physics/PhysicsSystem.cpp"
    "platforms/glfw/GlfwPlatform.cpp"
    "scene/Scene.cpp"
    "scene/SceneManager.cpp"
    "scripting/ScriptSystem.cpp"
    "scripting/sol2/bindings/Sol2API_Animation.cpp"
    "scripting/sol2/bindings/Sol2API_Audio.cpp"
    "scripting/sol2/bindings/Sol2API_Camera.cpp"
    "scripting/sol2/bindings/Sol2API_ECS.cpp"
    "scripting/sol2/bindings/Sol2API_Input.cpp"
    "scripting/sol2/bindings/Sol2API_Lights.cpp"
    "scripting/sol2/bindings/Sol2API_Math2.cpp"
    "scripting/sol2/bindings/Sol2API_Math.cpp"
    "scripting/sol2/bindings/Sol2API_Particles.cpp"
    "scripting/sol2/bindings/Sol2API_Physics.cpp"
    "scripting/sol2/bindings/Sol2API_Renderable.cpp"
    "scripting/sol2/bindings/Sol2API_Scene.cpp"
    "scripting/sol2/bindings/Sol2API_Spline.cpp"
    "scripting/sol2/bindings/Sol2API_Transform.cpp"
    "scripting/sol2/bindings/Sol2API_UI.cpp"
    "scripting/sol2/Sol2API.cpp"
    "scripting/sol2/Sol2ScriptBroker.cpp"
    "scripting/sol2/Sol2ScriptInstance.cpp"
    "scripting/sol2/Sol2State.cpp"
    "transform/TransformSystem.cpp"
    "UI/UIComponents/UIRenderComponent.cpp"
    "UI/UIComponents/UITransformComponent.cpp"
    "UI/UILayoutSystem.cpp"
    "window/glfw/LGlfwWindow.cpp"
    "window/WindowManager.cpp"
    ${ENGINE_SOURCES}
)

le_add_fmod(LaurelEyeEngine USE_STUDIO)

target_precompile_headers(LaurelEyeEngine
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/LaurelEyeEngine/pch.hpp
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

target_compile_definitions(LaurelEyeEngine
    PUBLIC PROJECT_ROOT_DIR="${PROJECT_SOURCE_DIR}"
)
