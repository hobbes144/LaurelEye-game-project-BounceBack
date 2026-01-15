#include "LaurelEyeEngine/graphics/MeshManager.h"
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
#include "LaurelEyeEngine/math/Transform.h"
#include <cstdint>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace LaurelEye::Graphics {

    MeshHandle MeshManager::createPrimitiveMesh(PrimitiveMeshType type) {
        if (primitiveMeshes.contains(type)) return primitiveMeshes[type];

        switch (type) {
        case PrimitiveMeshType::Sphere:
            return createSphereMesh("ManagedSpherePrimitive");
        case PrimitiveMeshType::Square:
            return createSquareMesh("ManagedSquarePrimitive");
        case PrimitiveMeshType::Cube:
            return createCubeMesh("ManagedCubePrimitive");
        default:
            throw std::logic_error("MESHMANAGER::CREATEPRIMITIVEMESH::INVALIDTYPE");
        }
    }

    MeshHandle MeshManager::createPrimitiveMesh(const std::string& name, PrimitiveMeshType type) {
        switch ( type ) {
        case PrimitiveMeshType::Sphere:
            return createSphereMesh(name);
        case PrimitiveMeshType::Square:
            return createSquareMesh(name);
        case PrimitiveMeshType::Cube:
            return createCubeMesh(name);
        default:
            throw std::logic_error("MESHMANAGER::CREATEPRIMITIVEMESH::INVALIDTYPE");
        }
    }

    MeshHandle MeshManager::createSphereMesh(
        const std::string& name,
        float radius,
        uint32_t stacks,
        uint32_t slices) {

        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        // Clamp minimum values so the sphere doesn’t degenerate
        stacks = std::max(2u, stacks);
        slices = std::max(3u, slices);

        const float PI = 3.14159265359f;

        // --- Generate vertices ---
        for ( uint32_t i = 0; i <= stacks; ++i ) {
            float v = float(i) / float(stacks); // 0..1
            float phi = v * PI;                 // 0..π

            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            for ( uint32_t j = 0; j <= slices; ++j ) {
                float u = float(j) / float(slices); // 0..1
                float theta = u * (2.0f * PI);      // 0..2π

                float sinTheta = sinf(theta);
                float cosTheta = cosf(theta);

                // Sphere param to Cartesian
                float x = sinPhi * cosTheta;
                float y = cosPhi;
                float z = sinPhi * sinTheta;

                MeshVertex vert;

                vert.position[0] = radius * x;
                vert.position[1] = radius * y;
                vert.position[2] = radius * z;

                // Normal is direction from center
                vert.normal[0] = x;
                vert.normal[1] = y;
                vert.normal[2] = z;

                vert.uv[0] = u;
                vert.uv[1] = 1.0f - v; // flip V for typical UV

                vertices.push_back(vert);
            }
        }

        // --- Generate triangle indices ---
        for ( uint32_t i = 0; i < stacks; ++i ) {
            for ( uint32_t j = 0; j < slices; ++j ) {
                uint32_t row1 = i * (slices + 1);
                uint32_t row2 = (i + 1) * (slices + 1);

                uint32_t i0 = row1 + j;
                uint32_t i1 = row1 + j + 1;
                uint32_t i2 = row2 + j;
                uint32_t i3 = row2 + j + 1;

                // Triangle 1
                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                // Triangle 2
                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        uint32_t countVerticies = static_cast<uint32_t>(vertices.size());
        uint32_t countIndicies = static_cast<uint32_t>(indices.size());
        return createMesh(name, vertices.data(), countVerticies, indices.data(), countIndicies);
    }

    void MeshManager::pushquad(std::vector<uint32_t>& indices,
                               uint32_t i, uint32_t j,
                               uint32_t k, uint32_t l,
                               uint32_t offset) {
        indices.push_back(offset + i);
        indices.push_back(offset + j);
        indices.push_back(offset + k);
        indices.push_back(offset + i);
        indices.push_back(offset + k);
        indices.push_back(offset + l);
    }

    void MeshManager::createFace(const Transform& tr,
                                 std::vector<MeshVertex>& vertices,
                                 std::vector<uint32_t>& indices,
                                 uint32_t indexOffset) {
        float verts[8] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
        float texcd[8] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

        Vector3 newPosition;
        Vector3 newNormals;
        Vector3 newTangents;

        // Four vertices to make a single face, with its own normal and
        // texture coordinates.
        for ( int i = 0; i < 8; i += 2 ) {
            newPosition = tr.getLocalMatrix() * Vector3(verts[i], verts[i + 1], 0.0f);
            newNormals = (tr.getRotation() * Vector3(0.0f, 0.0f, 1.0f)).normalized();
            newTangents = (tr.getRotation() * Vector3(1.0f, 0.0f, 0.0f)).normalized();
            vertices.push_back(MeshVertex{});
            vertices.back().position = newPosition;
            vertices.back().normal = newNormals;
            vertices.back().tangent = newTangents;
            vertices.back().uv[0] = texcd[i];
            vertices.back().uv[1] = texcd[i + 1];
        }

        pushquad(indices, 0, 1, 2, 3, indexOffset);
    }

    MeshHandle MeshManager::createSquareMesh(const std::string& name,
                                             float scale) {

        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        Transform I = Transform().setScaling(scale * 2.f);

        createFace(I, vertices, indices);

        uint32_t countVerticies = static_cast<uint32_t>(vertices.size());
        uint32_t countIndicies = static_cast<uint32_t>(indices.size());
        return createMesh(name, vertices.data(), countVerticies, indices.data(), countIndicies);
    }

    MeshHandle MeshManager::createCubeMesh(const std::string& name,
                                           float scale) {

        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        Transform I = Transform().setScaling(scale);
        float r90 = (float)(std::numbers::pi / 2.0f);

        // Six faces, each a rotation of a rectangle placed on the z axis.

        // Behind face
        createFace(I.setPosition(Vector3(.0f, .0f, .5f)),
                   vertices, indices);

        // Bottom face
        createFace(I.setPosition(Vector3(.0f, -.5f, .0f)).setRotation(Quaternion::fromEuler(r90, 0.0f, 0.0f)),
                   vertices, indices, 4);

        // Top face
        createFace(I.setPosition(Vector3(.0f, .5f, .0f)).setRotation(Quaternion::fromEuler(-r90, 0.0f, 0.0f)),
                   vertices, indices, 8);

        // Right face
        createFace(I.setPosition(Vector3(.5f, .0f, .0f)).setRotation(Quaternion::fromEuler(0.0f, r90, 0.0f)),
                   vertices, indices, 12);

        // Left face
        createFace(I.setPosition(Vector3(-.5f, .0f, .0f)).setRotation(Quaternion::fromEuler(0.0f, -r90, 0.0f)),
                   vertices, indices, 16);

        // Front face
        createFace(I.setPosition(Vector3(.0f, .0f, -.5f)).setRotation(Quaternion::fromEuler((float)std::numbers::pi, 0.0f, 0.0f)),
                   vertices, indices, 20);

        uint32_t countVerticies = static_cast<uint32_t>(vertices.size());
        uint32_t countIndicies = static_cast<uint32_t>(indices.size());
        return createMesh(name, vertices.data(), countVerticies, indices.data(), countIndicies);
    }

} // namespace LaurelEye::Graphics
