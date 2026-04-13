add_library(LaurelEyeGraphics STATIC)

add_library(LaurelEye::Graphics ALIAS LaurelEyeGraphics)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(HarfBuzz CONFIG REQUIRED)

target_sources(LaurelEyeGraphics PRIVATE
    "device/glfw/LGLDataBufferFactory.cpp"
    "device/glfw/LGLFramebufferFactory.cpp"
    "device/glfw/LGLRenderDevice.cpp"
    "device/glfw/LGLRenderState.cpp"
    "device/glfw/LGLTextureFactory.cpp"
    "device/glfw/LGLVertexArrayFactory.cpp"
    "FontManager.cpp"
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
    "renderpass/LocalLightsToonPass.cpp"
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
    "resources/Font.cpp"
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

target_precompile_headers(LaurelEyeGraphics
    PRIVATE
        ${LAURELEYE_ROOT_DIR}/include/LaurelEyeEngine/pch.hpp
)

target_include_directories(LaurelEyeGraphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(LaurelEyeGraphics
    PRIVATE
        LAURELEYE_GRAPHICS_BUILD
)

set_target_properties(LaurelEyeGraphics PROPERTIES
    OUTPUT_NAME "LaurelEyeGraphics"
    FOLDER "Engine"
)

target_compile_definitions(LaurelEyeGraphics PUBLIC PLATFORM_GLFW)

target_link_libraries(LaurelEyeGraphics
    PUBLIC
        glfw
        glad::glad
        assimp::assimp
        freetype
        harfbuzz::harfbuzz
)

add_compile_definitions(HB_HAVE_FREETYPE ON)

target_include_directories(LaurelEyeGraphics
    PUBLIC
        ${Stb_INCLUDE_DIR}            # provided by vcpkg
)
