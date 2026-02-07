#pragma once

#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Quaternion.h"
#include "LaurelEyeEngine/math/Transform.h"
#include "LaurelEyeEngine/math/Vector3.h"

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>

namespace LaurelEye::Platforms::Assimp::Utilities {
    inline Matrix4 convert(aiMatrix4x4 transform) {
        return Matrix4(transform.a1, transform.b1, transform.c1, transform.d1,
                       transform.a2, transform.b2, transform.c2, transform.d2,
                       transform.a3, transform.b3, transform.c3, transform.d3,
                       transform.a4, transform.b4, transform.c4, transform.d4);
    }

    inline Transform convertTransform(aiMatrix4x4 transform) {
        aiVector3D trans, scale;
        aiQuaternion rot;
        transform.Decompose(scale, rot, trans);
        const Vector3 newTrans = Vector3(trans.x, trans.y, trans.z);
        const Quaternion newRot = Quaternion(rot.w, rot.x, rot.y, rot.z);
        const Vector3 newScale = Vector3(scale.x, scale.y, scale.z);
        return Transform(newTrans, newRot, newScale);
    }
} // namespace LaurelEye::Platforms::Assimp::Utilities
