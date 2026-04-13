///
///
/// @file   DebugDrawSystem.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   11-03-2025
/// @brief  @file   RenderSystem.h
///
///

#pragma once

#include <vector>   // For std::vector

#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"
#include "LaurelEyeEngine/debugDraw/IDebugDrawComponent.h"
#include "LaurelEyeEngine/input/IInput.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Quaternion.h"

namespace LaurelEye::Physics {
    class PhysicsSystem;
}

namespace LaurelEye::Debug {

    enum class DebugDrawType {
        Line,
        AABB,
        Box,
        VelocityVector,
        Normal,
        Sphere,
        Circle,
        Capsule
        // ... more in the future
    };

    struct DebugLine {
        Vector3 start = Vector3(0);
        Vector3 end = Vector3(0);
        Vector3 color = Vector3(1);
    };

    struct DebugDrawCommand {
        DebugDrawType type;
        Vector3 position;
        Quaternion rotation;
        Vector3 size;
        Vector3 color;
        Vector3 direction; // for vectors/normals
        float radius;      // Sphere Circle and Capsule
    };

    class DebugDrawSystem final : public ISystem<IDebugDrawComponent> {

    public:
        DebugDrawSystem();

        /// @brief Initialize the debug draw system
        void initialize() override;
        /// @brief Updates draw debug data
        /// @param dt Delta time (in seconds) since the last update.
        void update(float dt) override;
        /// @brief Shutdown debug draw system
        void shutdown() override;

        void registerComponent(const ComponentPtr component) override;
        void deregisterComponent(const ComponentPtr component) override;

        const std::vector<DebugDrawCommand>& GetCommands() const { return frontBuffer; }
        void Clear();

        void ToggleEnabled() { enabled = !enabled; }

        void setMaxDebugLines(unsigned int max) { MaxDebugLines = max; }

        void populateWireFrameCommands(Physics::PhysicsSystem* physicsSystem, std::vector<DebugDrawCommand>& commands);

    private:
        bool enabled = false;
        unsigned int MaxDebugLines = 10000;
        unsigned int currentDebugLines = 0;
        Key hotKeyBind = Key::Tilde;

        std::vector<DebugDrawCommand> frontBuffer, backBuffer;

        std::shared_ptr<Graphics::DebugDrawRenderPass> dbrp;
        std::shared_ptr<Graphics::GeometryBuffer> lineGeoBuffer;

        void updateLineBufferObject();
        void dispatchNumLinesToRenderPass();

        void addLine(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                                      const Vector3& start, const Vector3& end,
                                      const Vector3& color);

        void addBox(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                    const Vector3& center, const Vector3& halfSize,
                    const Quaternion& rotation, const Vector3& color);

        void addArrow(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                      const Vector3& start, const Vector3& dir,
                      const Vector3& color);

        void addCapsule(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                        const Vector3& center, const Vector3& size, float radius,
                        const Quaternion& rotation, const Vector3& color);

        void addSphere(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                       const Vector3& center, float radius,
                       const Quaternion& rotation, const Vector3& color);



    };

}
