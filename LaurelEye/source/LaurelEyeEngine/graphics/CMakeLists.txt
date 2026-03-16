cmake_minimum_required(VERSION 3.8)

# Engine library
project (LaurelEyeEngine.Graphics VERSION 0.0.1 DESCRIPTION "Team Laurel GAM55x Engine Graphics lib")

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_library(LaurelEyeEngine.Graphics STATIC
    "device/glfw/LGLDataBufferFactory.cpp"
    "device/glfw/LGLFramebufferFactory.cpp"
    "device/glfw/LGLRenderDevice.cpp"
    "device/glfw/LGLRenderState.cpp"
    "device/glfw/LGLTextureFactory.cpp"
    "device/glfw/LGLVertexArrayFactory.cpp"
    "graphics_components/AmbientLightComponent.cpp"
    "graphics_components/CameraComponent.cpp"
    "graphics_components/DirectionalLightComponent.cpp"
    "graphics_components/IRenderComponent.cpp"
    "graphics_components/LightComponent.cpp"
    "graphics_components/PointLightComponent.cpp"
    "graphics_components/Renderable2DComponent.cpp"
    "graphics_components/Renderable3DComponent.cpp"
    "graphics_components/UIButtonComponent.cpp"
    "graphics_components/UIComponent.cpp"
    "graphics_components/UISliderComponent.cpp"
    "MeshManager.cpp"
    "MeshManagerPrimitives.cpp"
    "renderpass/DebugDrawRenderPass.cpp"
    "renderpass/DeferredRenderPass.cpp"
    "renderpass/GBufferPass.cpp"
    "renderpass/GBufferSkinnedPass.cpp"
    "renderpass/IRenderPass.cpp"
    "renderpass/LocalLightsPass.cpp"
    "renderpass/ParticleRenderPass.cpp"
    "renderpass/PointShadowSkinnedPass.cpp"
    "renderpass/ShadowPass.cpp"
    "renderpass/ShadowSkinnedPass.cpp"
    "renderpass/SingleBufferedDataPass.cpp"
    "renderpass/SinglePass.cpp"
    "renderpass/SinglePassShadow.cpp"
    "renderpass/SkydomePass.cpp"
    "renderpass/UIPass.cpp"
    "RenderStateSaver.cpp"
    "RenderSystem.cpp"
    "resources/GeometryBuffer.cpp"
    "resources/Material.cpp"
    "resources/Mesh.cpp"
    "resources/RenderResources.cpp"
    "resources/Shader.cpp"
    "ShaderManager.cpp"
    "ShadowManager.cpp"
    "SkeletonManager.cpp"
    "surface/glfw/LGlfwOpenGLWindowSurface.cpp"
)

target_precompile_headers(LaurelEyeEngine.Graphics
    PRIVATE
        ${LE_ENGINE_PROJECT_ROOT}/include/LaurelEyeEngine/pch.hpp
)

# Public headers
target_include_directories(LaurelEyeEngine.Graphics
    PUBLIC
        ${LE_ENGINE_PROJECT_ROOT}/include
        ${LE_ENGINE_PROJECT_ROOT}/external
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(LaurelEyeEngine.Graphics PROPERTIES PUBLIC_HEADER ${LE_ENGINE_PROJECT_ROOT}/include/LaurelEyeEngine/LaurelEyeEngine.h)

target_compile_definitions(LaurelEyeEngine.Graphics PUBLIC PLATFORM_GLFW)

if (CMAKE_VERSION VERSION_GREATER 3.20)
    set_property(TARGET LaurelEyeEngine.Graphics PROPERTY CXX_STANDARD 23)
endif()

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

target_link_libraries(LaurelEyeEngine.Graphics
    PUBLIC
        glfw
        glad::glad
        assimp::assimp
)

target_include_directories(LaurelEyeEngine.Graphics
    PUBLIC
        ${Stb_INCLUDE_DIR}            # provided by vcpkg
)

target_compile_definitions(LaurelEyeEngine.Graphics
    PUBLIC PROJECT_ROOT_DIR="${LE_ENGINE_PROJECT_ROOT}"
)
