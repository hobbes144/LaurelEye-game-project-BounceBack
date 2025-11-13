/*!****************************************************************************
 * \file   ShadowManager.cpp
 * \author Anish Murthy (anish.murthy@digipen.edu)
 * \date   04-21-2025
 *
 * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#include "LaurelEyeEngine/graphics/ShadowManager.h"

#include "LaurelEyeEngine/graphics/device/IRenderDevice.h"
#include "LaurelEyeEngine/graphics/RenderStateSaver.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/Lights.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/resources/Shadow.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include <cstdint>
#include <string>

// #include "Renderer.h"
// #include "RendererStateSaver.h"

namespace LaurelEye::Graphics {

    ShadowManager::ShadowManager(RenderResources* _rs) : rs(_rs), shadows(), shadowPass(ShadowPass()) {
        shadowPass.setup(*rs);
        DataBufferDesc d{
            DataBufferType::SSBO,
            DataBufferUpdateMode::Dynamic,
            sizeof(ShadowResource) * 100,
            DataBuffer::ShaderDataBinding};
        shadowResourcesBuffer = rs->createDataBuffer("shadowResources", d, "shadow");
    }

    void ShadowManager::update(const FrameContext& ctx, GlobalLights* globalLights, LocalLights* lights) {
        // Later add logic here for directional light, and for
        // updating changed lights

        // if ( momentWidth == -1 ) {
        //     setMomentWidth(10);
        // }

        const float epsilon = 0.01f;

        if ( isValidShadow(globalLights->sunLight.shadowIndex) ) {
            updateShadow(ctx, globalLights->sunLight.shadowIndex, globalLights->sunLight.direction);
        }

        for ( uint32_t i = 0; i < lights->pointLights.size(); ++i ) {
            const PointLight& light = lights->pointLights[i];
            if ( !isNoShadow(light.shadowIndex) ) {
                // Init lights if they don't already exist.
                if ( isShadowPending(light.shadowIndex) ) {
                    const uint32_t shadowIndex = addShadow(i, &lights->pointLights[i], ShadowDesc());

                    updateShadow(ctx, light.shadowIndex, light.position);
                    // updateMomentShadowMap(shadows[shadowIndex]);
                }
                else {
                    // Update shadows if any of the lights have moved.
                    const uint32_t shadowIndex = light.shadowIndex;
                    updateShadow(ctx, light.shadowIndex, light.position);
                    // updateMomentShadowMap(shadows[shadowIndex]);
                }
            }
        }
    }

    ShadowHandle ShadowManager::addShadow(
        uint32_t lightIndex, PointLight* light, const ShadowDesc& d) {
        ShadowHandle handle = static_cast<int>(shadows.size());
        light->shadowIndex = handle;

        ShadowInfo shadow{d, Vector3(), handle, ShadowSourceType::Point, "shadow_point"};

        ShadowResource r{};

        // Creating the Shadow FBOs
        TextureDesc shadowTexture;
        shadowTexture.debugName = "shadow_" + std::to_string(handle);
        shadowTexture.format = TextureFormat::RGBA32F;
        shadowTexture.width = d.resolution.width;
        shadowTexture.height = d.resolution.height;
        shadowTexture.mipMode = TextureMipMode::None;

        r.texture = rs->createTexture(
            "shadow_" + std::to_string(handle),
            shadowTexture,
            shadow.tag);

        FramebufferAttachmentDesc shadowTexAttachment;
        shadowTexAttachment.texture = r.texture;
        shadowTexAttachment.type = FramebufferAttachmentType::Color;

        FramebufferAttachmentDesc shadowDepthAttachment;
        shadowDepthAttachment.type = FramebufferAttachmentType::Depth;
        // shadowDepthAttachment.format = TextureFormat::DEPTH32F;
        shadowDepthAttachment.format = TextureFormat::DEPTH24;

        FramebufferDesc shadowFramebuffer;
        shadowFramebuffer.attachments.push_back(shadowTexAttachment);
        shadowFramebuffer.attachments.push_back(shadowDepthAttachment);
        shadowFramebuffer.size = d.resolution;

        r.framebuffer = rs->createFramebuffer(
            "shadow_" + std::to_string(handle),
            shadowFramebuffer,
            shadow.tag);

        rs->getRenderDevice().updateDataBufferSubData(shadowResourcesBuffer, sizeof(ShadowResource) * shadowResources.size(), sizeof(ShadowResource), &r);

        shadows.push_back(shadow);
        shadowResources.push_back(r);

        return shadow.handle;
    }

    ShadowHandle ShadowManager::addShadow(
        uint32_t lightIndex, DirectionalLight* light, const ShadowDesc& d) {
        ShadowHandle handle = static_cast<int>(shadows.size());
        light->shadowIndex = handle;

        ShadowInfo shadow{d, Vector3(), handle, ShadowSourceType::Directional, "shadow_sun"};

        ShadowResource r{};

        // Creating the Shadow FBOs
        TextureDesc shadowTexture;
        shadowTexture.debugName = "shadow_sun";
        shadowTexture.format = TextureFormat::RGBA32F;
        shadowTexture.width = d.resolution.width;
        shadowTexture.height = d.resolution.height;
        shadowTexture.mipMode = TextureMipMode::None;
        // shadowTexture.type = TextureType::TextureStorage2D;

        r.texture = rs->createTexture(
            "shadow_sun",
            shadowTexture,
            shadow.tag);

        FramebufferAttachmentDesc shadowTexAttachment;
        shadowTexAttachment.texture = r.texture;
        shadowTexAttachment.type = FramebufferAttachmentType::Color;

        FramebufferAttachmentDesc shadowDepthAttachment;
        shadowDepthAttachment.type = FramebufferAttachmentType::Depth;
        // shadowDepthAttachment.format = TextureFormat::DEPTH32F;
        shadowDepthAttachment.format = TextureFormat::DEPTH24;

        FramebufferDesc shadowFramebuffer;
        shadowFramebuffer.attachments.push_back(shadowTexAttachment);
        shadowFramebuffer.attachments.push_back(shadowDepthAttachment);
        shadowFramebuffer.size = d.resolution;

        r.framebuffer = rs->createFramebuffer(
            "shadow_sun",
            shadowFramebuffer,
            shadow.tag);

        rs->getRenderDevice().updateDataBufferSubData(shadowResourcesBuffer, sizeof(ShadowResource) * shadowResources.size(), sizeof(ShadowResource), &r);

        shadows.push_back(shadow);
        shadowResources.push_back(r);

        return shadow.handle;
    }

    void ShadowManager::updateShadow(
        const FrameContext& ctx, ShadowHandle h, const Vector3& posDir) {
        IRenderDevice* device = &rs->getRenderDevice();
        RenderStateSaver renderState = RenderStateSaver(device);

        ShadowInfo& shadow = shadows[h];
        ShadowResource& r = shadowResources[h];

        device->bindFramebufferBase(r.framebuffer);

        glViewport(0, 0, shadow.desc.resolution.width, shadow.desc.resolution.height);
        GLfloat bkColor[4];
        glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        device->clear();
        glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);

        if ( shadow.source == ShadowSourceType::Point ) {
            const Matrix4 viewMatrix = Matrix4::lookAt(
                posDir,
                posDir - Vector3(0.0f, 1.0f, 0.0f),
                Vector3(0.0f, 0.0f, -1.0f));
            const Matrix4 projectionMatrix = Matrix4::perspective(
                45.0f * 3.14159f / 180.0f,
                1.0f,
                params.near,
                params.far);
            shadowPass.setProperties(
                ShadowPass::Properties{projectionMatrix * viewMatrix});
            r.shadowMatrix =
                Matrix4::translation(0.5f) * Matrix4::scale(0.5f) *
                projectionMatrix * viewMatrix;
        }
        else {
            const Matrix4 viewMatrix = Matrix4::lookAt(
                (-posDir) * 100.0f,
                0.0f,
                Vector3(0.0f, 1.0f, 0.0f));
            const Matrix4 projectionMatrix = Matrix4::orthographic(
                -50.0f, 50.0f,
                -50.0f, 50.0f,
                params.near,
                params.far);
            shadowPass.setProperties(
                ShadowPass::Properties{projectionMatrix * viewMatrix});
            r.shadowMatrix =
                Matrix4::translation(0.5f) * Matrix4::scale(0.5f) *
                projectionMatrix * viewMatrix;
        }
        shadowPass.execute(ctx);

        rs->getRenderDevice().updateDataBufferSubData(shadowResourcesBuffer, sizeof(ShadowResource) * h, sizeof(ShadowResource), &r);
    }

    void ShadowManager::bindShadowBuffer() const {
        rs->getRenderDevice().bindDataBufferBase(shadowResourcesBuffer);
    }

    // void ShadowManager::calculateMomentWeights(int width) {
    //     std::vector<float> weights;
    //     weights.push_back(width);
    //
    //     weights.reserve(width * 2 + 2);
    //     for ( int i = -width; i < width + 1; ++i ) {
    //         weights.push_back(expf(-(1 / 2) * powf(i / (width / 2), 2)));
    //     }
    //
    //     glGenBuffers(1, &momentUBO);
    //     glBindBuffer(GL_UNIFORM_BUFFER, momentUBO);
    //     glBindBufferBase(GL_UNIFORM_BUFFER, 3, momentUBO);
    //     glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * weights.size(), weights.data(), GL_STATIC_DRAW);
    //     glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // }

    // void ShadowManager::setMomentWidth(int width) {
    //     momentWidth = width;
    //     calculateMomentWeights(momentWidth);
    // }

    // void ShadowManager::updateMomentShadowMap(ShadowInfo& shadow) {
    //     auto tempTexture = renderGraphBuilder->getTexture("moment_shadow_temp");
    //
    //     glBindImageTexture(
    //         0, shadow.fboInfo.textureID, 0,
    //         GL_FALSE, 0,
    //         GL_READ_ONLY,
    //         TextureManager::getInstance().getTextureInfo(
    //                                          shadow.fboInfo.textureID)
    //             .format);
    //
    //     glBindImageTexture(
    //         1, tempTexture, 0,
    //         GL_FALSE, 0,
    //         GL_WRITE_ONLY,
    //         TextureManager::getInstance().getTextureInfo(
    //                                          tempTexture)
    //             .format);
    //
    //     momentShadowMapPass.computeHor(RenderPass::ComputeDispatchSize(1024 / 16, 1024 / 1, 1));
    //
    //     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //
    //     glBindImageTexture(
    //         0, tempTexture, 0,
    //         GL_FALSE, 0,
    //         GL_READ_ONLY,
    //         TextureManager::getInstance().getTextureInfo(
    //                                          tempTexture)
    //             .format);
    //
    //     glBindImageTexture(
    //         1, shadow.fboInfo.momentTextureID, 0,
    //         GL_FALSE, 0,
    //         GL_WRITE_ONLY,
    //         TextureManager::getInstance().getTextureInfo(
    //                                          shadow.fboInfo.momentTextureID)
    //             .format);
    //
    //     momentShadowMapPass.computeVert(RenderPass::ComputeDispatchSize(1024 / 1, 1024 / 16, 1));
    // }

} // namespace LaurelEye::Graphics
