#pragma once

#include "LaurelEyeEngine/graphics/resources/Viewport.h"

namespace LaurelEye::Graphics {

    enum class BlendEquation {
        Add,
    };

    enum class BlendFactor {
        One,
    };

    struct BlendState {
        bool enabled = false;
        BlendEquation equation = BlendEquation::Add;
        BlendFactor srcFactor = BlendFactor::One;
        BlendFactor destFactor = BlendFactor::One;

        bool operator==(const BlendState& other) const {
            return enabled == other.enabled &&
                   equation == other.equation &&
                   srcFactor == other.srcFactor &&
                   destFactor == other.destFactor;
        }
    };

    enum class DepthFunction {
        Less,
    };

    struct DepthState {
        bool testEnabled = true;
        bool writeEnabled = true;
        DepthFunction func = DepthFunction::Less;

        bool operator==(const DepthState& other) const {
            return testEnabled == other.testEnabled &&
                   writeEnabled == other.writeEnabled &&
                   func == other.func;
        }
    };

    struct RenderState {
        Viewport viewport;
        BlendState blendState;
        DepthState depthState;

        RenderState() : viewport(), blendState(), depthState() {}
        RenderState(const Viewport& vp, const BlendState& bs = BlendState{},
                    const DepthState& ds = DepthState{}) : viewport(vp), blendState(bs), depthState(ds) {};
    };

} // namespace LaurelEye::Graphics
