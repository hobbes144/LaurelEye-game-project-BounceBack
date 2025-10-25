#pragma once

#include "LaurelEyeEngine/math/Matrix4.h"

namespace LaurelEye::Graphics {
    struct Camera {
        Matrix4 projectionMatrix;
        Matrix4 viewMatrix;
        Matrix4 inverseViewMatrix;
        float exposure;
    };

}
