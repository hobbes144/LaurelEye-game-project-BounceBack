#pragma once

#include "LaurelEyeEngine/math/Matrix4.h"

#include <assimp/matrix4x4.h>

namespace LaurelEye::Platforms::Assimp::Utilities {
    inline Matrix4 convert(aiMatrix4x4 transform) {
        return Matrix4(transform.a1, transform.b1, transform.c1, transform.d1,
                       transform.a2, transform.b2, transform.c2, transform.d2,
                       transform.a3, transform.b3, transform.c3, transform.d3,
                       transform.a4, transform.b4, transform.c4, transform.d4);
    }
} // namespace LaurelEye
