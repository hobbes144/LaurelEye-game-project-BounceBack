#pragma once

#include "LaurelEyeEngine/math/Matrix4.h"

namespace LaurelEye::Graphics {

    enum class CameraProjectionType {
        Perspective,
        Orthographic
    };

    // Currently unimplemented, but here for reference and maybe future impl.
    // enum class PerspectiveResizePolicy {
    //     FixedVerticalFov,     // shows more on ultrawide; shows less on portrait
    //     FixedHorizontalFov,   // shows more vertical on portrait (usually undesired)
    //     CropToReferenceAspect // shows never more than reference; bars as needed
    // };
    //
    // enum class OrthoResizePolicy {
    //     FixedHeight, // common; no extra vertical on portrait
    //     FixedWidth,
    //     CropToReferenceAspect // strict; bars as needed
    // };

    struct Camera {
        Matrix4 projectionMatrix;
        Matrix4 viewMatrix;
        Matrix4 inverseViewMatrix;
        float exposure;
    };

}
