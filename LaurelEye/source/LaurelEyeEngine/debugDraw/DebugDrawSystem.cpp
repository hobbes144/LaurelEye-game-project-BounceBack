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

namespace LaurelEye::Debug {

    DebugDrawSystem::DebugDrawSystem() {}

    void DebugDrawSystem::initialize() {

        dbrp = context->getService<Graphics::RenderSystem>()->retrieveDebugDrawRenderPass();
        assert(dbrp != nullptr);

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
        physicsSystem->populateWireFrameCommands(backBuffer);
        updateLineBufferObject();
        assert(dbrp != nullptr);
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
        Vector3 end = start + dir;
        addLine(attr, start, end, color);

        // Small arrowhead
        Vector3 right = dir.normalized().cross({0, 1, 0}).normalized() * 0.1f;
        Vector3 up = right.cross(dir.normalized()) * 0.1f;
        addLine(attr, end, end - dir.normalized() * 0.2f + right, color);
        addLine(attr, end, end - dir.normalized() * 0.2f - right, color);
        addLine(attr, end, end - dir.normalized() * 0.2f + up, color);
        addLine(attr, end, end - dir.normalized() * 0.2f - up, color);
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
} // namespace LaurelEye::Debug
