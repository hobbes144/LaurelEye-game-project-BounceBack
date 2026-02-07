/// @file   SinglePass.cpp
/// @author Martin Chow
/// @par    **DigiPen Email**
///     martin.chow@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of SinglePass render pass

#include "LaurelEyeEngine/graphics/renderpass/ShadowSkinnedPass.h"

#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye::Graphics {

    void ShadowSkinnedPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/Shadows.frag\n../../../assets/shaders/ShadowsSkinned.vert");

        DataBufferDesc shadowDataBuffer{};
        shadowDataBuffer.type = DataBufferType::UBO;
        shadowDataBuffer.sizeBytes = sizeof(Properties);
        shadowDataBuffer.bindingPoint = DataBuffer::CameraDataBinding;
        propertiesHandle = rs.createDataBuffer("shadow_pass_properties", shadowDataBuffer, "shadow");
    }

    void ShadowSkinnedPass::execute(const FrameContext& ctx) {

        shader->use();

        if ( propertiesDirty ) {
            ctx.device.updateDataBufferSubData(
                propertiesHandle,
                0, sizeof(Matrix4),
                properties.ProjViewMatrix.getData());
        }

        ctx.device.bindDataBufferBase(propertiesHandle);

        // Action
        drawSkinnedRenderables(ctx, shader);
    }

    void ShadowSkinnedPass::setProperties(Properties _properties) {
        properties = _properties;
        propertiesDirty = true;
    }


    void ShadowSkinnedPass::drawSkinnedRenderables(const FrameContext& ctx, std::shared_ptr<Shader> shader) {
        if ( isValidDataBuffer(propertiesHandle) ) ctx.device.bindDataBufferBase(propertiesHandle);

        for ( const auto& icomponent : ctx.objects ) {
            if ( !icomponent->isActive() ) continue;
            // Remove this when we fix FrameContext to send only renderables:
            auto component = (IRenderableComponent*)icomponent;

            if ( isValidMesh(component->GetMesh()) ) {
                component->GetMaterial()->setProperty(
                    "ModelMatrix",
                    component->GetBoundTransform()->getWorldTransform().getLocalMatrix());
                component->GetMaterial()->setProperty(
                    "InvModelMatrix",
                    component->GetBoundTransform()->getWorldTransform().getInverseLocalMatrix());

                component->GetMaterial()->apply(shader);

                if ( !isValidDataBuffer(component->skinDataBuffer) || !isValidDataBuffer(component->animationDataBuffer) ) {
                    shader->setInt("isSkinned", 0);
                }
                else {
                    ctx.device.bindDataBufferBase(component->skinDataBuffer);
                    ctx.device.bindDataBufferBase(component->animationDataBuffer);
                    shader->setInt("isSkinned", 1);
                }

                ctx.device.bindVertexArray(component->vao);
                glDrawElements(GL_TRIANGLES, component->indexCount, GL_UNSIGNED_INT, 0);
            }
        }
    }
} // namespace LaurelEye::Graphics
