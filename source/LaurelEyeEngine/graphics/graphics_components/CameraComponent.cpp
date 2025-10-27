/// @file   AmbientLightComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief  Camera component implementation file.

#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/resources/Camera.h"

#include <source_location>

inline void warn_impl(std::string_view expr,
                      std::string_view msg = {},
                      std::source_location loc = std::source_location::current()) {
    std::cerr << "[warn] " << loc.file_name() << ':' << loc.line()
              << " in " << loc.function_name()
              << " — condition '" << expr << "' failed"
              << (msg.empty() ? "" : ": ") << msg << '\n';
}

#define WARN_IF(cond, ...)                                              \
    do {                                                                \
        if ( (cond) ) warn_impl(#cond, std::string_view{__VA_ARGS__}); \
    } while ( 0 )

#ifndef NDEBUG
#define DEBUG_WARN_IF(cond, ...) WARN_IF(cond, __VA_ARGS__)
#else
#define DEBUG_WARN_IF(cond, ...) ((void)0)
#endif

namespace LaurelEye::Graphics {

    CameraComponent::CameraComponent()
        : IRenderPropertyComponent(RenderComponentType::PropertyCamera),
          name("Camera"),
          position(Vector3()),
          rotation(Quaternion(0.0f, 0.0f, 1.0f, 0.0f)),
          cameraData() {};

    CameraComponent::~CameraComponent() = default;

    void CameraComponent::setPositionRotation(const Vector3& _position, const Quaternion& _rotation) {
        position = _position;
        rotation = _rotation;
        updateViewMatrix();
    }

    void CameraComponent::setPosition(const Vector3& _position) {
        position = _position;
        updateViewMatrix();
    }

    void CameraComponent::setRotation(const Quaternion& _rotation) {
        rotation = _rotation;
        updateViewMatrix();
    }

    const Camera* CameraComponent::getCameraDataPtr() const {
        return &cameraData;
    }

    void CameraComponent::updateViewMatrix() {
        // Extract basis vectors (Right, Up, Forward)
        forward = rotation.forward();
        up = rotation.up();
        // right = rotation.right();

        // Compute view matrix using LookAt
        cameraData.viewMatrix = Matrix4::lookAt(position, position - forward, up);
        cameraData.inverseViewMatrix = Matrix4::inverse(cameraData.viewMatrix);
    }

    void CameraComponent::setPerspectiveProjection(
        const float fov,
        const float aspectRatio,
        const float near,
        const float far) {
        cameraData.projectionMatrix = Matrix4::perspective(fov, aspectRatio, near, far);

        return;
    }

    void CameraComponent::setOrthographicProjection(
        const float left,
        const float right,
        const float bottom,
        const float top,
        const float near,
        const float far) {
        cameraData.projectionMatrix = Matrix4::orthographic(
            left, right, bottom, top, near, far);

        return;
    }

    const Matrix4& CameraComponent::getViewMatrix() {
        return cameraData.viewMatrix;
    }

    const Matrix4& CameraComponent::getInverseViewMatrix() {
        return cameraData.inverseViewMatrix;
    }

    const Matrix4& CameraComponent::getProjectionMatrix() {
        return cameraData.projectionMatrix;
    }

} // namespace LaurelEye::Graphics
