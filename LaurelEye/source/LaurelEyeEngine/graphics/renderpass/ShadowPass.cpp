/// @file   SinglePass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of SinglePass render pass

#include "LaurelEyeEngine/graphics/renderpass/ShadowPass.h"

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye::Graphics {

    void ShadowPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/Shadows.frag\n../../../assets/shaders/Shadows.vert");

        DataBufferDesc shadowDataBuffer{};
        shadowDataBuffer.type = DataBufferType::UBO;
        shadowDataBuffer.sizeBytes = sizeof(Properties);
        shadowDataBuffer.bindingPoint = DataBuffer::CameraDataBinding;
        propertiesHandle = rs.createDataBuffer("shadow_pass_properties", shadowDataBuffer, "shadow");
    }

    void ShadowPass::execute(const FrameContext& ctx) {

        shader->use();

        if (propertiesDirty) {
            ctx.device.updateDataBufferSubData(
                propertiesHandle,
                0, sizeof(Matrix4),
                properties.ProjViewMatrix.getData());
        }

        ctx.device.bindDataBufferBase(propertiesHandle);

        // Action
        drawRenderables(ctx, shader);
    }

    void ShadowPass::setProperties(Properties _properties) {
        properties = _properties;
        propertiesDirty = true;
    }

} // namespace LaurelEye::Graphics
