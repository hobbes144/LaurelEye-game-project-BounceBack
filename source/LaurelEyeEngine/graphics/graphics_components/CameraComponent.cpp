/// @file   AmbientLightComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief  Camera component implementation file.

#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"

namespace LaurelEye::Graphics {

    CameraComponent::CameraComponent()
        : IRenderPropertyComponent(RenderComponentType::PropertyCamera), name("Camera"), position(Vector3()),
          rotation(Quaternion(0.0f, 0.0f, 1.0f, 0.0f)),
          exposure(1.0f) {

          };

    void CameraComponent::updateViewMatrix() {
        // Extract basis vectors (Right, Up, Forward)
        forward = rotation.forward();
        up = rotation.up();
        right = rotation.right();

        // Compute view matrix using LookAt
        viewMatrix = Matrix4::lookAt(position, position + forward, up);
        inverseViewMatrix = Matrix4::inverse(viewMatrix);
    }

    void CameraComponent::setPerspectiveProjection(
        const float fov,
        const float aspectRatio,
        const float near,
        const float far) {
        projectionMatrix = Matrix4::perspective(fov, aspectRatio, near, far);

        return;
    }

    void CameraComponent::setOrthographicProjection(
        const float left,
        const float right,
        const float bottom,
        const float top,
        const float near,
        const float far) {
        projectionMatrix = Matrix4::orthographic(
            left, right, bottom, top, near, far);

        return;
    }

    const Matrix4& CameraComponent::getViewMatrix() {
        return viewMatrix;
    }

    const Matrix4& CameraComponent::getInverseViewMatrix() {
        return inverseViewMatrix;
    }

    const Matrix4& CameraComponent::getProjectionMatrix() {
        return projectionMatrix;
    }

} // namespace LaurelEye::Graphics
