/// @file   UIPass.h
/// @author Daoming Wang (KeitherJinn@Outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-30-2025
/// @brief Ui render pass definition.
#pragma once
#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include <glad/glad.h>

namespace LaurelEye::Graphics {
    class UIPass : public IRenderPass {
    public:
        struct BlendFactor {
            GLenum equation = GL_FUNC_ADD;
            GLenum srcFactor = GL_ONE;
            GLenum destFactor = GL_ONE;

            bool operator==(const BlendFactor& other) const {
                return equation == other.equation &&
                       srcFactor == other.srcFactor &&
                       destFactor == other.destFactor;
            }
        };

        /// @brief Default constructor.
        UIPass()
            : IRenderPass() {}
        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

        void setBlendFactor(const BlendFactor& factor) {
            blendFactor = factor;
        }

    private:
        BlendFactor blendFactor;
    };

} // namespace LaurelEye::Graphics
