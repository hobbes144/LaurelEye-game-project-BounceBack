/// @file   IRenderPass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of IRenderPass

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/device/IRenderDevice.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"

#include <memory>

namespace LaurelEye::Graphics {

    void IRenderPass::drawRenderables(const FrameContext& ctx, std::shared_ptr<Shader> shader) {
        if ( isValidDataBuffer(propertiesHandle) ) ctx.device.bindDataBufferBase(propertiesHandle);

        for ( const auto& icomponent : ctx.objects ) {
            if ( !icomponent->isActive() ) continue;
            // Remove this when we fix FrameContext to send only renderables:
            auto component = (IRenderableComponent*)icomponent;
            component->GetMaterial()->setProperty(
                "ModelMatrix",
                component->GetBoundTransform()->getWorldTransform().getLocalMatrix());
            component->GetMaterial()->setProperty(
                "InvModelMatrix",
                component->GetBoundTransform()->getWorldTransform().getInverseLocalMatrix());

            component->GetMaterial()->apply(shader);

            if (isValidMesh(component->GetMesh())) {
                // component->GetMesh()->draw();
                ctx.device.bindVertexArray(component->vao);
                glDrawElements(GL_TRIANGLES, component->indexCount, GL_UNSIGNED_INT, 0);
            }
        }
    }

} // namespace LaurelEye::Graphics
