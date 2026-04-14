///
/// @file   DebugDrawSystem.cpp
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   11-03-2025
/// @brief
///

#include "LaurelEyeEngine/debugDraw/DebugDrawSystem.h"
#include "LaurelEyeEngine/graphics/renderpass/DebugDrawRenderPass.h"
#include "LaurelEyeEngine/input/InputManager.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/physics/interfaces/IRigidBody.h"

#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::Debug {

    DebugDrawSystem::DebugDrawSystem() {}

    void DebugDrawSystem::initialize() {

        dbrp = context->getService<Graphics::RenderSystem>()->retrieveDebugDrawRenderPass();
        if (!dbrp) {
            LE_ERROR("DebugDraw", "Failed to initialize debug draw system");
            return;
        }

        // --- Create GPU buffer once ---
        if ( !lineGeoBuffer ) {
            Graphics::GeometryBuffer::ModifiableAttributes tempAttributes;

            tempAttributes[Graphics::GeometryBuffer::AttributeType::Position] = {
                std::vector<float>(MaxDebugLines * 2 * 3),
                3, GL_FLOAT, GL_FALSE, 0};

            tempAttributes[Graphics::GeometryBuffer::AttributeType::Color] = {
                std::vector<float>(MaxDebugLines * 2 * 4),
                4, GL_FLOAT, GL_FALSE, 0};

            lineGeoBuffer = Graphics::GeometryBuffer::create(tempAttributes, "DebugLineBuffer", false);

            if ( dbrp ) {
                dbrp->updateLineBuffer(lineGeoBuffer);
                dbrp->updateNumLines(0);
            }
        }
    }

    void DebugDrawSystem::update(float dt) {
        if (!dbrp) {
            return;
        }
        currentDebugLines = 0;

        // Check Hotkey Toggle
        if ( context->getService<InputManager>()->isKeyPressed(hotKeyBind) ) {
            ToggleEnabled();
        }

        // Do nothing if not enabled
        if ( !enabled ) {
            if ( !frontBuffer.empty() || !backBuffer.empty() ) {
                Clear();
            }
            return;
        }

        for ( auto* comp : components ) {
            // Do Something
        }

        // Get a reference to the Physics System
        Physics::PhysicsSystem* physicsSystem = context->getService<Physics::PhysicsSystem>();

        std::swap(frontBuffer, backBuffer);
        backBuffer.clear();
        populateWireFrameCommands(physicsSystem, backBuffer);
        updateLineBufferObject();
        dbrp->updateNumLines(currentDebugLines);
    }

    void DebugDrawSystem::shutdown() {
    }

    void DebugDrawSystem::registerComponent(const ComponentPtr component) {
        // Add to Active List
        ISystem<IDebugDrawComponent>::registerComponent(component);
    }

    void DebugDrawSystem::deregisterComponent(const ComponentPtr component) {
        // Remove from Active List
        ISystem<IDebugDrawComponent>::deregisterComponent(component);
    }



    void DebugDrawSystem::populateWireFrameCommands(Physics::PhysicsSystem* physicsSystem, std::vector<Debug::DebugDrawCommand>& commands) {

        for ( Physics::PhysicsBodyBaseComponent* const pbbc : physicsSystem->components ) {

            std::shared_ptr<Physics::ICollider> body = pbbc->GetCollider();
            if ( !body ) continue;

            LaurelEye::TransformComponent* transform = body->GetBoundTransform();
            if ( !transform ) continue;

            Physics::PhysicsBodyData bodyData = pbbc->GetBodyData();
            Physics::CollisionShapePhys shapeDef = bodyData.shapeDefinition;

            const Vector3 objectPos = transform->getWorldPosition();
            const Quaternion objectRot = transform->getWorldRotation();
            const Vector3 scale = transform->getWorldScale();

            // --- Apply compound offset transform ---
            const Vector3 offset = bodyData.centerOfMass;
            const Vector3 offsetRot = bodyData.rotationOfCenter;

            Vector3 pos = objectPos + (objectRot * offset);
            Quaternion rot = objectRot * Quaternion::fromEuler(offsetRot);

            // --- Shape wireframe ---
            Debug::DebugDrawCommand cmd;
            cmd.position = pos;
            cmd.rotation = rot;
            cmd.color = {0.0f, 1.0f, 0.0f}; // Green for collider outlines

            switch ( shapeDef.type ) {
                case Physics::CollisionShapePhys::ShapeType::Box: {
                    cmd.type = Debug::DebugDrawType::Box;
                    cmd.size = shapeDef.size * scale; // element-wise scaling
                    break;
                }

                case Physics::CollisionShapePhys::ShapeType::Sphere: {
                    cmd.type = Debug::DebugDrawType::Sphere;
                    float avgScale = (scale.x + scale.y + scale.z) / 3.0f;
                    cmd.radius = shapeDef.radius * avgScale;
                    break;
                }

                case Physics::CollisionShapePhys::ShapeType::Capsule: {
                    cmd.type = Debug::DebugDrawType::Capsule;
                    float radialScale = (scale.x + scale.z) * 0.5f; // average XZ for radius
                    cmd.radius = shapeDef.radius * radialScale;
                    cmd.size = {0.0f, shapeDef.height * scale.y, 0.0f};
                    break;
                }

                case Physics::CollisionShapePhys::ShapeType::Cylinder: {
                    cmd.type = Debug::DebugDrawType::Cylinder;
                    float radialScale = (scale.x + scale.z) * 0.5f;
                    cmd.radius = shapeDef.radius * radialScale;
                    cmd.size = {0.0f, shapeDef.height * scale.y, 0.0f};
                    break;
                }

                case Physics::CollisionShapePhys::ShapeType::Mesh:
                default:
                    LE_ASSERT("Physics", false , "ERROR::LAURELEYE::PHYSICS_SYSTEM::POPULATE_WIRE_FRAME_COMMANDS::INVALID_SHAPE");
                    break;
            }

            switch ( bodyData.type ) {
            case Physics::BodyType::Static:
                cmd.color = {1.0f, 1.0f, 0.0f};
                break;
            case Physics::BodyType::Kinematic:
                cmd.color = {0.25f, 0.75f, 0.0f};
                break;
            case Physics::BodyType::Dynamic:
                cmd.color = {0.0f, 1.0f, 0.0f};
                break;
            case Physics::BodyType::Ghost:
                cmd.color = {0.0f, 0.5f, 0.5f};
                break;
            default:
                cmd.color = {1.0f, 1.0f, 1.0f};
                break;
            }

            commands.push_back(cmd);
            //Do This Next
            // --- Velocity Vector ---
            if ( auto rigid = std::dynamic_pointer_cast<Physics::IRigidBody>(body) ) {
                const Vector3 velocity = rigid->GetLinearVelocity();
                if ( velocity.magnitudSquared() > 0.0001f ) {
                    Debug::DebugDrawCommand velCmd;
                    velCmd.type = Debug::DebugDrawType::VelocityVector;
                    velCmd.position = pos;
                    velCmd.direction = velocity.normalized();
                    velCmd.size = {velocity.magnitude(), 0.0f, 0.0f};
                    velCmd.color = {1.0f, 0.0f, 0.0f}; // Red for velocity
                    commands.push_back(velCmd);
                }
            }

            /*
            // --- Force Vector (if available) ---
            if ( body->HasAppliedForces() ) {
                Vector3 totalForce = body->GetAccumulatedForce();
                if ( totalForce.magnitudSquared() > 0.0001f ) {
                    Debug::DebugDrawCommand forceCmd;
                    forceCmd.type = Debug::DebugDrawType::VelocityVector; // reuse arrow
                    forceCmd.position = pos;
                    forceCmd.direction = totalForce.normalized();
                    forceCmd.size = {totalForce.magnitude() * 0.01f, 0.0f, 0.0f}; // scale for visibility
                    forceCmd.color = {1.0f, 1.0f, 0.0f};                 // Yellow for forces
                    commands.push_back(forceCmd);
                }
            }
            */
            //Do This Last
            /*
            // --- Surface Normals  ---
            if ( body->HasSurfaceNormals() ) {
                const auto& normals = body->GetSurfaceNormals();
                for ( const auto& n : normals ) {
                    Debug::DebugDrawCommand normalCmd;
                    normalCmd.type = Debug::DebugDrawType::Normal;
                    normalCmd.position = n.origin;
                    normalCmd.direction = n.direction;
                    normalCmd.size = {0.5f, 0, 0};        // length of normal
                    normalCmd.color = {0.0f, 0.0f, 1.0f}; // Blue for normals
                    commands.push_back(normalCmd);
                }
            }
            */

        }

    }

    void DebugDrawSystem::Clear() {
        frontBuffer.clear();
        backBuffer.clear();
        currentDebugLines = 0;

        // Inform render pass immediately to stop drawing
        if ( dbrp ) {
            dbrp->updateNumLines(0);
        }

        // Optionally clear GPU buffer contents (safe fallback)
        if ( lineGeoBuffer ) {
            // empty attribute arrays
            std::vector<float> emptyPos;
            std::vector<float> emptyCol;
            // position expects 3 floats per vertex, color expects 4 per vertex
            lineGeoBuffer->updateVertexAttribute(Graphics::GeometryBuffer::AttributeType::Position, emptyPos);
            lineGeoBuffer->updateVertexAttribute(Graphics::GeometryBuffer::AttributeType::Color, emptyCol);
        }
    }

    void DebugDrawSystem::updateLineBufferObject() {
        if ( frontBuffer.empty() )
            return;

        // --- Gather vertex data ---
        Graphics::GeometryBuffer::ModifiableAttributes attributes;
        attributes[Graphics::GeometryBuffer::AttributeType::Position] = {{}, 3, GL_FLOAT, GL_FALSE, 0};
        attributes[Graphics::GeometryBuffer::AttributeType::Color] = {{}, 4, GL_FLOAT, GL_FALSE, 0};

        for ( const auto& cmd : frontBuffer ) {
            if ( currentDebugLines >= MaxDebugLines )
                break;
            switch ( cmd.type ) {
            case DebugDrawType::Line:
                addLine(attributes, cmd.position, cmd.position + (cmd.rotation * cmd.size), cmd.color);
                break;

            case DebugDrawType::Box:
            case DebugDrawType::AABB:
                if ( currentDebugLines >= MaxDebugLines - 12 ) break;
                addBox(attributes, cmd.position, cmd.size * 0.5f, cmd.rotation, cmd.color);
                break;

            case DebugDrawType::VelocityVector:
                if ( currentDebugLines >= MaxDebugLines - 3 ) break;
                addArrow(attributes, cmd.position, cmd.direction, cmd.color);
                break;

            case DebugDrawType::Normal:
                addLine(attributes, cmd.position, cmd.position + cmd.direction * cmd.size.x, cmd.color);
                break;
            case DebugDrawType::Capsule:
                // Call a function to add capsule wireframe lines
                addCapsule(attributes, cmd.position, cmd.size, cmd.radius, cmd.rotation, cmd.color);
                break;
            case DebugDrawType::Cylinder:
                addCylinder(attributes, cmd.position, cmd.size, cmd.radius, cmd.rotation, cmd.color);
                break;
            case DebugDrawType::Sphere:
                //Ensure there is enough room
                if ( currentDebugLines >= MaxDebugLines - 80 ) break;

                addSphere(attributes, cmd.position, cmd.radius, cmd.rotation, cmd.color);
                break;
            default:
                break;
            }
        }

        // --- Upload data ---
        if ( lineGeoBuffer->hasAttribute(Graphics::GeometryBuffer::AttributeType::Position) ) {
            lineGeoBuffer->updateVertexAttribute(
                Graphics::GeometryBuffer::AttributeType::Position,
                attributes[Graphics::GeometryBuffer::AttributeType::Position].data);
        }

        if ( lineGeoBuffer->hasAttribute(Graphics::GeometryBuffer::AttributeType::Color) ) {
            lineGeoBuffer->updateVertexAttribute(
                Graphics::GeometryBuffer::AttributeType::Color,
                attributes[Graphics::GeometryBuffer::AttributeType::Color].data);
        }
    }

    // Adds a simple line
    void DebugDrawSystem::addLine(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                                  const Vector3& start, const Vector3& end,
                                  const Vector3& color) {
        currentDebugLines++;

        // Start Pos
        attr[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(start.x);
        attr[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(start.y);
        attr[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(start.z);

        // End Pos
        attr[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(end.x);
        attr[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(end.y);
        attr[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(end.z);

        // Color Start
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(color.x);
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(color.y);
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(color.z);
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(1.0f);

        // Color End
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(color.x);
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(color.y);
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(color.z);
        attr[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(1.0f);
    }

    // Draws an axis-aligned or oriented box using 12 lines
    void DebugDrawSystem::addBox(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                                 const Vector3& center, const Vector3& halfSize,
                                 const Quaternion& rotation, const Vector3& color) {
        Vector3 corners[8] = {
            {-halfSize.x, -halfSize.y, -halfSize.z},
            { halfSize.x, -halfSize.y, -halfSize.z},
            { halfSize.x,  halfSize.y, -halfSize.z},
            {-halfSize.x,  halfSize.y, -halfSize.z},
            {-halfSize.x, -halfSize.y,  halfSize.z},
            { halfSize.x, -halfSize.y,  halfSize.z},
            { halfSize.x,  halfSize.y,  halfSize.z},
            {-halfSize.x,  halfSize.y,  halfSize.z}
        };

        for ( auto& c : corners )
            c = rotation * c + center;

        const int edges[12][2] = {
            {0, 1},
            {1, 2},
            {2, 3},
            {3, 0},
            {4, 5},
            {5, 6},
            {6, 7},
            {7, 4},
            {0, 4},
            {1, 5},
            {2, 6},
            {3, 7}
        };

        for ( int i = 0; i < 12; ++i )
            addLine(attr, corners[edges[i][0]], corners[edges[i][1]], color);
    }

    // Adds a velocity or direction arrow
    void DebugDrawSystem::addArrow(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                                   const Vector3& start, const Vector3& dir,
                                   const Vector3& color) {
        if ( dir.magnitudSquared() < 1e-6f )
            return; // nothing to draw

        Vector3 forward = dir.normalized();
        Vector3 end = start + dir;
        addLine(attr, start, end, color);

        // Pick a safe up vector
        Vector3 upRef = std::abs(forward.y) > 0.99f ? Vector3(1, 0, 0)
                                                    : Vector3(0, 1, 0);

        Vector3 right = forward.cross(upRef).normalized() * 0.1f;
        Vector3 up = right.cross(forward).normalized() * 0.1f;

        Vector3 back = forward * 0.2f;

        addLine(attr, end, end - back + right, color);
        addLine(attr, end, end - back - right, color);
        addLine(attr, end, end - back + up, color);
        addLine(attr, end, end - back - up, color);
    }


    // Debug drawing the dark wizard created for capsules so I could see them, have Nick/Anish look at it
    void DebugDrawSystem::addCapsule(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                                     const Vector3& center, const Vector3& size, float radius,
                                     const Quaternion& rotation, const Vector3& color) {
        if ( currentDebugLines >= MaxDebugLines )
            return;

        const float PI = 3.14159265358979323846f;
        const int segments = 16; // around circumference
        const int latSegs = 6;   // hemisphere latitude slices

        const float cylinderHeight = std::max(0.0f, size.y);
        const float totalHeight = cylinderHeight + 2.0f * radius;

        // sphere centers are offset from the capsule center by half the cylinder height
        float sphereCenterOffset = cylinderHeight * 0.5f;

        // sphere centers in world (apply rotation)
        Vector3 topCenter = center + rotation * Vector3(0.0f, sphereCenterOffset, 0.0f);
        Vector3 bottomCenter = center - rotation * Vector3(0.0f, sphereCenterOffset, 0.0f);

        // --- Draw circles at top and bottom (equator of each sphere / cylinder caps) ---
        auto drawCircle = [&](const Vector3& c, float r, bool connectToNextRing, const std::vector<Vector3>* nextRing = nullptr) {
            // circle in local XZ plane, transformed by rotation
            std::vector<Vector3> points;
            points.resize(segments);
            for ( int i = 0; i < segments; ++i ) {
                float a = (float)i / (float)segments * 2.0f * PI;
                Vector3 local = Vector3(std::cos(a) * r, 0.0f, std::sin(a) * r);
                points[i] = rotation * local + c;
            }
            // ring lines
            for ( int i = 0; i < segments; ++i ) {
                int ni = (i + 1) % segments;
                if ( currentDebugLines >= MaxDebugLines ) return;
                addLine(attr, points[i], points[ni], color);
            }
            // connect to next ring if provided (used for connecting lat rings / cylinder)
            if ( connectToNextRing && nextRing && nextRing->size() == (size_t)segments ) {
                for ( int i = 0; i < segments; ++i ) {
                    if ( currentDebugLines >= MaxDebugLines ) return;
                    addLine(attr, points[i], (*nextRing)[i], color);
                }
            }
        };

        // Precompute ring points per latitude for top hemisphere and bottom hemisphere
        std::vector<std::vector<Vector3>> topRings;
        std::vector<std::vector<Vector3>> bottomRings;

        // Equator ring (where cylinder meets spheres)
        std::vector<Vector3> topEquator(segments), bottomEquator(segments);
        for ( int i = 0; i < segments; ++i ) {
            float a = (float)i / (float)segments * 2.0f * PI;
            Vector3 local = Vector3(std::cos(a) * radius, 0.0f, std::sin(a) * radius);
            topEquator[i] = rotation * local + topCenter;
            bottomEquator[i] = rotation * local + bottomCenter;
        }

        topRings.push_back(topEquator);
        bottomRings.push_back(bottomEquator);

        // Hemisphere latitude rings (excluding pole) for top and bottom
        for ( int lat = 1; lat <= latSegs; ++lat ) {
            float t = (float)lat / (float)latSegs; // 0..1
            // theta from 0 (equator) -> PI/2 (pole)
            float theta = t * (PI * 0.5f);
            float ringR = std::cos(theta) * radius;
            float yOff = std::sin(theta) * radius; // distance from sphere center to ring along local +Y

            // Top hemisphere ring center is topCenter + (0, yOff, 0)
            Vector3 ringCenterTop = topCenter + rotation * Vector3(0.0f, yOff, 0.0f);
            Vector3 ringCenterBottom = bottomCenter - rotation * Vector3(0.0f, yOff, 0.0f);

            std::vector<Vector3> ringTop(segments), ringBottom(segments);
            for ( int i = 0; i < segments; ++i ) {
                float a = (float)i / (float)segments * 2.0f * PI;
                Vector3 local = Vector3(std::cos(a) * ringR, 0.0f, std::sin(a) * ringR);
                ringTop[i] = rotation * local + ringCenterTop;
                ringBottom[i] = rotation * local + ringCenterBottom;
            }
            topRings.push_back(std::move(ringTop));
            bottomRings.push_back(std::move(ringBottom));
        }

        // --- Draw cylinder body: connect topEquator to bottomEquator and draw both rings ---
        for ( int i = 0; i < segments; ++i ) {
            if ( currentDebugLines >= MaxDebugLines ) return;
            int ni = (i + 1) % segments;
            addLine(attr, topEquator[i], topEquator[ni], color);
            if ( currentDebugLines >= MaxDebugLines ) return;
            addLine(attr, bottomEquator[i], bottomEquator[ni], color);
            // connect top->bottom to show cylinder sides
            if ( currentDebugLines >= MaxDebugLines ) return;
            addLine(attr, topEquator[i], bottomEquator[i], color);
        }

        // --- Draw hemisphere rings and longitudinal connections ---
        for ( size_t ringIdx = 0; ringIdx + 1 < topRings.size(); ++ringIdx ) {
            const auto& ringA = topRings[ringIdx];
            const auto& ringB = topRings[ringIdx + 1];
            // ring circles
            for ( int i = 0; i < segments; ++i ) {
                if ( currentDebugLines >= MaxDebugLines ) return;
                int ni = (i + 1) % segments;
                addLine(attr, ringA[i], ringA[ni], color);
            }
            // connect corresponding points between consecutive rings (longitudinal lines)
            for ( int i = 0; i < segments; ++i ) {
                if ( currentDebugLines >= MaxDebugLines ) return;
                addLine(attr, ringA[i], ringB[i], color);
            }
        }

        for ( size_t ringIdx = 0; ringIdx + 1 < bottomRings.size(); ++ringIdx ) {
            const auto& ringA = bottomRings[ringIdx];
            const auto& ringB = bottomRings[ringIdx + 1];
            for ( int i = 0; i < segments; ++i ) {
                if ( currentDebugLines >= MaxDebugLines ) return;
                int ni = (i + 1) % segments;
                addLine(attr, ringA[i], ringA[ni], color);
            }
            for ( int i = 0; i < segments; ++i ) {
                if ( currentDebugLines >= MaxDebugLines ) return;
                addLine(attr, ringA[i], ringB[i], color);
            }
        }
    }

    void DebugDrawSystem::addCylinder(
        Graphics::GeometryBuffer::ModifiableAttributes& attr,
        const Vector3& position,
        const Vector3& size,
        float radius,
        const Quaternion& rotation,
        const Vector3& color) {
        constexpr int segments = 16;
        constexpr float twoPi = 6.28318530718f;

        const float halfHeight = std::max(0.0f, size.y);

        auto rotatePoint = [&](const Vector3& p) -> Vector3 {
            return rotation * p; // assumes your Quaternion supports q * v
        };

        for ( int i = 0; i < segments; ++i ) {
            int next = (i + 1) % segments;

            float a0 = (static_cast<float>(i) / segments) * twoPi;
            float a1 = (static_cast<float>(next) / segments) * twoPi;

            Vector3 top0Local(
                std::cos(a0) * radius,
                halfHeight,
                std::sin(a0) * radius);

            Vector3 top1Local(
                std::cos(a1) * radius,
                halfHeight,
                std::sin(a1) * radius);

            Vector3 bot0Local(
                std::cos(a0) * radius,
                -halfHeight,
                std::sin(a0) * radius);

            Vector3 bot1Local(
                std::cos(a1) * radius,
                -halfHeight,
                std::sin(a1) * radius);

            Vector3 top0 = position + rotatePoint(top0Local);
            Vector3 top1 = position + rotatePoint(top1Local);
            Vector3 bot0 = position + rotatePoint(bot0Local);
            Vector3 bot1 = position + rotatePoint(bot1Local);

            // Top ring
            addLine(attr, top0, top1, color);

            // Bottom ring
            addLine(attr, bot0, bot1, color);

            // Side lines
            addLine(attr, top0, bot0, color);
        }
    }

    void DebugDrawSystem::addSphere(Graphics::GeometryBuffer::ModifiableAttributes& attr,
                                    const Vector3& center, float radius,
                                    const Quaternion& rotation, const Vector3& color) {
        if ( currentDebugLines >= MaxDebugLines )
            return;

        const float PI = 3.14159265358979323846f;
        const int latSegs = 5; // horizontal slices
        const int lonSegs = 8; // vertical slices

        // Latitude rings (Y axis)
        for ( int lat = 0; lat <= latSegs; ++lat ) {
            float v = (float)lat / (float)latSegs; // 0..1
            float theta = v * PI;                  // 0..PI

            float y = std::cos(theta) * radius;
            float ringR = std::sin(theta) * radius;

            std::vector<Vector3> ring(lonSegs);

            for ( int lon = 0; lon < lonSegs; ++lon ) {
                float u = (float)lon / (float)lonSegs;
                float phi = u * 2.0f * PI;

                Vector3 local(
                    std::cos(phi) * ringR,
                    y,
                    std::sin(phi) * ringR);

                ring[lon] = rotation * local + center;
            }

            // Connect ring
            for ( int i = 0; i < lonSegs; ++i ) {
                if ( currentDebugLines >= MaxDebugLines ) return;
                int ni = (i + 1) % lonSegs;
                addLine(attr, ring[i], ring[ni], color);
            }
        }

        // Longitude rings (around Y axis)
        for ( int lon = 0; lon < lonSegs; ++lon ) {
            float u = (float)lon / (float)lonSegs;
            float phi = u * 2.0f * PI;

            std::vector<Vector3> ring(latSegs + 1);

            for ( int lat = 0; lat <= latSegs; ++lat ) {
                float v = (float)lat / (float)latSegs;
                float theta = v * PI;

                Vector3 local(
                    std::cos(phi) * std::sin(theta) * radius,
                    std::cos(theta) * radius,
                    std::sin(phi) * std::sin(theta) * radius);

                ring[lat] = rotation * local + center;
            }

            for ( int i = 0; i < latSegs; ++i ) {
                if ( currentDebugLines >= MaxDebugLines ) return;
                addLine(attr, ring[i], ring[i + 1], color);
            }
        }
    }


} // namespace LaurelEye::Debug
