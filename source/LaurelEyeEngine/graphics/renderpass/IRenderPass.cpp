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

#include <memory>

namespace LaurelEye::Graphics {

    void IRenderPass::drawRenderables(const FrameContext& ctx, std::shared_ptr<Shader> shader) {
        if ( isValidDataBuffer(properties) ) ctx.device.bindDataBufferBase(properties);

        for ( const auto& icomponent : ctx.objects ) {
            // Remove this when we fix FrameContext to send only renderables:
            auto component = (IRenderableComponent*)icomponent;
            component->GetMaterial()->setProperty(
                "ModelMatrix",
                component->GetBoundTransform()->getWorldTransform().getLocalMatrix());
            component->GetMaterial()->setProperty(
                "InvModelMatrix",
                component->GetBoundTransform()->getWorldTransform().getInverseLocalMatrix());

            component->GetMaterial()->apply(shader);
            component->GetMesh()->draw();
        }
    }

} // namespace LaurelEye::Graphics
