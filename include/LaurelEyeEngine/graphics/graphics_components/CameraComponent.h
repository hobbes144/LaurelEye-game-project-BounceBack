/// @file   CameraComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief Camera component to create render scene cameras.
///
/// All cameras in the scene are to be registered using this component.
/// Currently only one camera is supported, but once render SceneManager is
/// done, we can use the SceneManager handle which camera is used per draw
/// call.

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/IRenderPropertyComponent.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Quaternion.h"

namespace LaurelEye::Graphics {

    /// @class CameraComponent
    /// @brief Camera component to handle render scene cameras
    ///
    /// Warning: only the first camera will be used during render calls until
    /// render scene management is done.
    class CameraComponent : public IRenderPropertyComponent {

    public:
        CameraComponent();

        /// @brief Get camera name
        ///
        /// @return Camera name.
        const std::string& getName() { return name; }
        /// @brief Set camera name.
        ///
        /// @param _name Camera name.
        void setName(const std::string& _name) { name = _name; }

        // CameraComponent(std::string _name)
        //     : name(_name), position(Vector3()),
        //       rotation(Quaternion(0.0f, 0.0f, 1.0f, 0.0f)),
        //       exposure(1.0f) {
        //     forward = rotation.forward();
        //     up = rotation.up();
        //     right = rotation.right();
        //     viewMatrix = Matrix4::lookAt(position, position + forward, up);
        //     inverseViewMatrix = Matrix4::inverse(viewMatrix);
        // };

        /* Component functions */

        /// @brief Update Camera's View matrix using the current camera
        /// position and rotation.
        ///
        /// This need only be called when the position or rotation changes.
        ///
        /// NOTE: This does not modify the camera's GPU buffers.
        virtual void updateViewMatrix();

        /// @brief Get View matrix of the camera.
        ///
        /// @return View matrix.
        const Matrix4& getViewMatrix();
        /// @brief Get Inverse View matrix of the camera.
        ///
        /// @return Inverse View matrix.
        const Matrix4& getInverseViewMatrix();
        /// @brief Get Projection matrix of the camera.
        ///
        /// @return Projection matrix.
        const Matrix4& getProjectionMatrix();

        // TODO: Add getViewProjMatrix function to reduce indirection?
        // TODO: Move GPU specific calls to private and make RenderSystem a friend.
        // TODO: Add accessors for position and rotation.

        /// @brief Set if the camera buffer was initialized.
        ///
        /// This is meant to store if the buffer for this camera was created.
        /// Typically we do not want to keep updating the camera buffers for
        /// such little data. It is better to store the cameras as separate
        /// buffers and swap the binding points during draw calls so that
        /// there's no pointless CPU->GPU data transfers (which are very slow).
        ///
        /// @param status Initialization status.
        void setInitStatus(bool status) { isInit = status; }
        /// @brief Get if the camera buffer was initialized.
        ///
        /// @return initialization status.
        bool getInitStatus() const { return isInit; }

        /// @brief Set camera DataBuffer.
        ///
        /// @param handle The GPU DataBufferHandle of the camera.
        void setCameraBufferHandle(DataBufferHandle handle) { cameraBufferHandle = handle; }
        /// @brief Get camera DataBuffer.
        ///
        /// @return The GPU DataBufferHandle of the camera.
        DataBufferHandle getCameraBufferHandle() const { return cameraBufferHandle; }

        /// @brief Get camera exposure setting.
        ///
        /// NOTE: Currently non functional.
        ///
        /// @return Exposure of the camera.
        const float& getExposure() { return exposure; }
        /// @brief Set the camera exposure.
        ///
        /// @param _exposure Camera exposure.
        void setExposure(float _exposure) { exposure = _exposure; }

        /* Utility functions */
        /// @brief Set Camera Projection matrix to a perspective projection.
        ///
        /// @param fov Camera fov.
        /// @param aspectRatio Window aspect ratio.
        /// @param near Near render plane.
        /// @param far Far render plane.
        void setPerspectiveProjection(
            const float fov,
            const float aspectRatio,
            const float near,
            const float far);
        /// @brief Set Camera Projection matrix to an Orthographic projection.
        ///
        /// @param left Left render plane.
        /// @param right Right render plane.
        /// @param bottom Bottom render plane.
        /// @param top Top render plane.
        /// @param near Near render plane.
        /// @param far Far render plane.
        void setOrthographicProjection(
            const float left,
            const float right,
            const float bottom,
            const float top,
            const float near,
            const float far);

    private:
        std::string name;
        Matrix4 viewMatrix;
        Matrix4 projectionMatrix;
        Matrix4 inverseViewMatrix;
        Vector3 position = Vector3();
        Quaternion rotation = Quaternion();

        DataBufferHandle cameraBufferHandle;

        float exposure;

        Vector3 forward;
        Vector3 right;
        Vector3 up;

        bool isInit = false;
    };

} // namespace LaurelEye::Graphics
