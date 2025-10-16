/// @file   Mesh.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Mesh

/*!****************************************************************************
 * \file   Mesh.cpp
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
 *   (zaldivaralvarez@gmail.com) (Advising and Base Code)
 * \par    **Course**
 *    CS529
 * \date   10-15-2024
 *
 * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/

#include "LaurelEyeEngine/graphics/resources/Mesh.h"

namespace LaurelEye::Graphics {

    std::unordered_map<Mesh::Type, std::shared_ptr<Mesh>> Mesh::shapeMeshes;

    Mesh::Mesh(const std::string& name) : name(name) {}

    Mesh::Mesh(
        const std::string& name,
        Attributes attrData,
        const GLsizei interleavedStride) {

        GeometryBuffer::ModifiableAttributes geometryBufferAttr;
        prepareAttributeData(geometryBufferAttr, attrData, interleavedStride);

        this->name = name;
        geometryBuffer = GeometryBuffer::create(
            geometryBufferAttr,
            name,
            (interleavedStride != 0));
    }

    Mesh::Mesh(
        const std::string& name,
        Attributes attrData,
        const std::vector<unsigned int>& indices,
        const GLsizei interleavedStride) {

        GeometryBuffer::ModifiableAttributes geometryBufferAttr;
        prepareAttributeData(geometryBufferAttr, attrData, interleavedStride);

        this->name = name;
        this->indices = indices;
        geometryBuffer = GeometryBuffer::create(
            geometryBufferAttr,
            indices,
            name,
            (interleavedStride != 0));
    }

    void Mesh::prepareAttributeData(
        GeometryBuffer::ModifiableAttributes& triangleBufferData,
        const Mesh::Attributes& attrData,
        const GLsizei stride) {

        for ( const auto& [attr, info] : attrData ) {
            triangleBufferData[attr] = {
                info.first,
                info.second,
                GL_FLOAT,
                GL_FALSE,
                stride};
        }
    }

    void Mesh::setAttributeData(
        GeometryBuffer::AttributeType& type,
        const std::vector<float>& data,
        int componentsPerVertex,
        const GLsizei interleavedStride = 0) {

        if ( geometryBuffer )
            geometryBuffer->updateVertexAttribute(type, data);

        else {
            GeometryBuffer::ModifiableAttributes attributeMap;
            Mesh::Attributes attrData;
            attrData[type] = {data, componentsPerVertex};
            prepareAttributeData(attributeMap, attrData, interleavedStride);
            geometryBuffer = GeometryBuffer::create(attributeMap, name, false);
        }
    }

    void Mesh::setVertexData(
        const std::shared_ptr<GeometryBuffer>& geometryBuffer) {
        this->geometryBuffer = geometryBuffer;
        this->indices = geometryBuffer->getIndexData();
    }

    void Mesh::setIndices(const std::vector<unsigned int>& newIndices) {
        this->geometryBuffer->updateIndices(newIndices);
        this->indices = newIndices;
    }

    std::unique_ptr<Mesh> Mesh::clone() const {
        return std::make_unique<Mesh>(*this);
    }

    size_t Mesh::getVertexCount() const {
        return geometryBuffer ? geometryBuffer->getVertexCount() : 0;
    }

    size_t Mesh::getIndexCount() const {
        return geometryBuffer ? geometryBuffer->getIndexCount() : 0;
    }

    bool Mesh::hasAttribute(GeometryBuffer::AttributeType attr) const {
        return geometryBuffer && geometryBuffer->hasAttribute(attr);
    }

    const std::string& Mesh::getName() const {
        return name;
    }

    void Mesh::draw(GLenum mode) {
        if ( !geometryBuffer ) return;
        geometryBuffer->bind();

        /* Removed reference to renderer to avoid circular dependency issues.
         * This really should have been in Renderer, or some other encapsulating class
         * Todo: Move mesh draw OpenGL calls to different class.
         */
        if ( this->hasAttribute(GeometryBuffer::AttributeType::Position) ) {
            if ( this->getIndexCount() > 0 ) {
                glDrawElements(mode, geometryBuffer->getIndexCount(), GL_UNSIGNED_INT, 0);
            }
            else {
                glDrawArrays(mode, 0, geometryBuffer->getVertexCount());
            }
        }

        // For efficiency, removing unbind.
        // We will be running draw calls multiple times per frame because we have
        // multiple passes now. Doing an unbind every pass just to bind it again is
        // very inefficient.
        //
        // geometryBuffer->unbind();
    }

    void Mesh::drawEBOTrisWithoutBind() {
        glDrawElements(GL_TRIANGLES, geometryBuffer->getIndexCount(), GL_UNSIGNED_INT, 0);
    }

    std::shared_ptr<Mesh> Mesh::createSquareMesh(const std::string& name, float scale) {
        Transform I = Transform().setScaling(scale * 2.0f);

        Attributes squareMeshData;
        std::vector<unsigned int> indices;

        std::pair<Mesh::Attributes, std::vector<unsigned int>> faceData;

        faceData = createFace(I);
        squareMeshData = faceData.first;
        indices = faceData.second;

        std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(
            name + "_Mesh",
            squareMeshData,
            indices,
            static_cast<GLsizei>(0));
        return newMesh;
    }

    // std::shared_ptr<Mesh> Mesh::createSquareMesh(const std::string& name)
    //{
    //   // Generate vertex data for a rectangle
    //   // We'll use a unit rectangle centered at the origin
    //   std::vector<float> vertices = {
    //     // Position (x, y, z)
    //     -0.5f, -0.5f, 0.0f,      0.0f, 0.0f,  // Bottom-left
    //      0.5f, -0.5f, 0.0f,      1.0f, 0.0f,  // Bottom-right
    //      0.5f,  0.5f, 0.0f,      1.0f, 1.0f,  // Top-right
    //     -0.5f,  0.5f, 0.0f,      0.0f, 1.0f   // Top-left
    //   };
    //
    //
    //   // Generate indices for two triangles
    //   std::vector<unsigned int> indices = {
    //       0, 1, 2,  // First triangle
    //       2, 3, 0   // Second triangle
    //   };
    //
    //   Mesh::Attributes newMeshData;
    //
    //   newMeshData[GeometryBuffer::AttributeType::Position] = {
    //     vertices,
    //     3
    //   };
    //
    //   newMeshData[GeometryBuffer::AttributeType::TexCoord] = {
    //     vertices,
    //     2
    //   };
    //
    //   std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(
    //     name + "_Mesh",
    //     newMeshData,
    //     indices,
    //     static_cast<GLsizei>(5 * sizeof(float)));
    //   return newMesh;
    // }

    Mesh::Attributes Mesh::combineAttributes(const std::vector<Attributes>& attributesList) {
        Attributes combinedAttributes;
        bool isFirst = true;
        for ( const auto& currentAttributesBlock : attributesList ) {
            if ( isFirst ) {
                for ( const auto& [attributeType, dataPair] : currentAttributesBlock ) {
                    combinedAttributes[attributeType].first = dataPair.first;
                    combinedAttributes[attributeType].second = dataPair.second;
                }
                isFirst = false;
                continue;
            }
            for ( const auto& [attributeType, dataPair] : currentAttributesBlock ) {
                combinedAttributes[attributeType].first.insert(
                    combinedAttributes[attributeType].first.end(),
                    dataPair.first.begin(), dataPair.first.end());
            }
        }

        return combinedAttributes;
    }

    void Mesh::combineIndices(unsigned int previousVertices, std::vector<unsigned int>& destIndices, std::vector<unsigned int> newIndices) {
        for ( auto index : newIndices ) {
            destIndices.push_back(index + previousVertices);
        }
    }

    std::unordered_map<std::string, std::shared_ptr<Mesh>> Mesh::loadedMeshes;

    void Mesh::pushquad(std::vector<unsigned int>& Tri, int i, int j, int k, int l) {
        Tri.push_back(i);
        Tri.push_back(j);
        Tri.push_back(k);
        Tri.push_back(i);
        Tri.push_back(k);
        Tri.push_back(l);
    }

    std::pair<Mesh::Attributes, std::vector<unsigned int>> Mesh::createFace(const Transform& tr) {
        Attributes newMeshData;
        std::vector<unsigned int> indices;

        newMeshData[GeometryBuffer::AttributeType::Position] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 3);
        newMeshData[GeometryBuffer::AttributeType::Normal] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 3);
        newMeshData[GeometryBuffer::AttributeType::TexCoord] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 2);
        newMeshData[GeometryBuffer::AttributeType::Tangent] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 3);

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
            newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(newPosition.x);
            newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(newPosition.y);
            newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(newPosition.z);
            newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(newNormals.x);
            newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(newNormals.y);
            newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(newNormals.z);
            newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(texcd[i]);
            newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(texcd[i + 1]);
            newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(newTangents.x);
            newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(newTangents.y);
            newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(newTangents.z);
        }

        pushquad(indices, 0, 1, 2, 3);

        return std::pair<Mesh::Attributes, std::vector<unsigned int>>(newMeshData, indices);
    }

    std::shared_ptr<Mesh> Mesh::createSphereMesh(const std::string& name, const int n) {
        Attributes newMeshData;
        std::vector<unsigned int> indices;

        newMeshData[GeometryBuffer::AttributeType::Position] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 3);
        newMeshData[GeometryBuffer::AttributeType::Normal] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 3);
        newMeshData[GeometryBuffer::AttributeType::TexCoord] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 2);
        newMeshData[GeometryBuffer::AttributeType::Tangent] =
            std::pair<std::vector<float>, GLint>(std::vector<float>(), 3);

        float d = 2.0f * PI / float(n * 2);

        for ( int i = 0; i <= n * 2; i++ ) {
            float s = i * 2.0f * PI / float(n * 2);
            for ( int j = 0; j <= n; j++ ) {
                float t = j * PI / float(n);
                float x = cos(s) * sin(t);
                float y = sin(s) * sin(t);
                float z = cos(t);

                newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(x);
                newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(y);
                newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(z);
                newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(x);
                newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(y);
                newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(z);
                newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(s / (2 * PI));
                newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(t / PI);
                newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(-sin(s));
                newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(cos(s));
                newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(0.0);

                if ( i > 0 && j > 0 ) {
                    pushquad(indices, (i - 1) * (n + 1) + (j - 1),
                             (i - 1) * (n + 1) + (j),
                             (i) * (n + 1) + (j),
                             (i) * (n + 1) + (j - 1));
                }
            }
        }

        std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(
            name + "_Mesh",
            newMeshData,
            indices,
            static_cast<GLsizei>(0));
        return newMesh;
    }

    std::shared_ptr<Mesh> Mesh::createCubeMesh(const std::string& name) {
        Transform I = Transform(Vector3(), Quaternion(), Vector3(1.0f));

        Attributes cubeMeshData;
        std::vector<unsigned int> indices;

        std::pair<Mesh::Attributes, std::vector<unsigned int>> faceData;
        std::vector<Attributes> faceAttributes;

        // Six faces, each a rotation of a rectangle placed on the z axis.

        // Behind face
        faceData = createFace(I.setPosition(Vector3(.0f, .0f, .5f)));
        faceAttributes.push_back(faceData.first);
        indices = faceData.second;

        float r90 = PI / 2;

        // Bottom face
        faceData = createFace(I.setPosition(Vector3(.0f, -.5f, .0f)).setRotation(Quaternion::fromEuler(r90, 0.0f, 0.0f)));
        faceAttributes.push_back(faceData.first);
        combineIndices(4, indices, faceData.second);

        // Top face
        faceData = createFace(I.setPosition(Vector3(.0f, .5f, .0f)).setRotation(Quaternion::fromEuler(-r90, 0.0f, 0.0f)));
        faceAttributes.push_back(faceData.first);
        combineIndices(8, indices, faceData.second);

        // Right face
        faceData = createFace(I.setPosition(Vector3(.5f, .0f, .0f)).setRotation(Quaternion::fromEuler(0.0f, r90, 0.0f)));
        faceAttributes.push_back(faceData.first);
        combineIndices(12, indices, faceData.second);

        // Left face
        faceData = createFace(I.setPosition(Vector3(-.5f, .0f, .0f)).setRotation(Quaternion::fromEuler(0.0f, -r90, 0.0f)));
        faceAttributes.push_back(faceData.first);
        combineIndices(16, indices, faceData.second);

        // Front face
        faceData = createFace(I.setPosition(Vector3(.0f, .0f, -.5f)).setRotation(Quaternion::fromEuler(PI, 0.0f, 0.0f)));
        faceAttributes.push_back(faceData.first);
        combineIndices(20, indices, faceData.second);

        cubeMeshData = combineAttributes(faceAttributes);

        std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(
            name + "_Mesh",
            cubeMeshData,
            indices,
            static_cast<GLsizei>(0));
        return newMesh;
    }

    // std::shared_ptr<Mesh> Mesh::createCubeMesh(const std::string& name)
    //{
    //   // Generate vertex data for a rectangle
    //   // We'll use a unit rectangle centered at the origin
    //   std::vector<float> vertices = {
    //     // Box corners in normalized coordinates (-0.5 to 0.5)
    //     -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-left-Back    //  0 A
    //      0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-right-Back   //  1 B
    //      0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-right-Back      //  2 C
    //     -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  // Top-left-Back       //  3 D
    //     -0.5f, -0.5f, 0.5f,   0.0f, 0.0f,  // Bottom-left-Front      //  4 E
    //      0.5f, -0.5f, 0.5f,   1.0f, 0.0f,  // Bottom-right-Front     //  5 F
    //      0.5f,  0.5f, 0.5f,   1.0f, 1.0f,  // Top-right-Front        //  6 G
    //     -0.5f,  0.5f, 0.5f,   0.0f, 1.0f,  // Top-left-Front         //  7 H
    //     -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left-Back    //  8 A
    //      0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-right-Back   //  9 B
    //      0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  // Top-right-Back      // 10 C
    //     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-left-Back       // 11 D
    //     -0.5f, -0.5f, 0.5f,   0.0f, 1.0f,  // Bottom-left-Front      // 12 E
    //      0.5f, -0.5f, 0.5f,   1.0f, 1.0f,  // Bottom-right-Front     // 13 F
    //      0.5f,  0.5f, 0.5f,   1.0f, 0.0f,  // Top-right-Front        // 14 G
    //     -0.5f,  0.5f, 0.5f,   0.0f, 0.0f,  // Top-left-Front         // 15 H
    //     -0.5f, -0.5f, 0.5f,   1.0f, 0.0f,  // Bottom-left-Front      // 16 E
    //      0.5f, -0.5f, 0.5f,   0.0f, 0.0f,  // Bottom-right-Front     // 17 F
    //      0.5f,  0.5f, 0.5f,   0.0f, 1.0f,  // Top-right-Front        // 18 G
    //     -0.5f,  0.5f, 0.5f,   1.0f, 1.0f,  // Top-left-Front         // 19 H
    //   };
    //
    //
    //   // Generate indices for two triangles
    //   std::vector<unsigned int> indices = {
    //     0, 1, 2,  // Bottom Right Back triangle     // 0
    //     2, 3, 0,  // Top left Back triangle          // 3
    //     4, 5, 6,  // Bottom Right Front triangle          // 6
    //     6, 7, 4,  // Top Left Front triangle           // 9
    //     8, 9, 13,  // Bottom Right Bottom triangle     // 12
    //     13, 12, 8,  // Top left Bottom triangle          // 15
    //     15, 14, 10,  // Bottom Right Top triangle          // 18
    //     10, 11, 15,  // Top Left Top triangle           // 21
    //     16, 8, 11,  // Bottom Right Left triangle     // 24
    //     11, 19, 16,  // Top left Left triangle          // 27
    //     10, 9, 17,  // Bottom Right Right triangle          // 30
    //     17, 18, 10,  // Top Left Right triangle           // 33
    //   };
    //
    //   Mesh::Attributes newMeshData;
    //
    //   newMeshData[GeometryBuffer::AttributeType::Position] = {
    //     vertices,
    //     3
    //   };
    //
    //   newMeshData[GeometryBuffer::AttributeType::TexCoord] = {
    //     vertices,
    //     2
    //   };
    //
    //   std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(
    //     name + "_Mesh",
    //     newMeshData,
    //     indices,
    //     static_cast<GLsizei>(5 * sizeof(float)));
    //   return newMesh;
    // }

    std::shared_ptr<Mesh> Mesh::createMesh(const std::string& name, Type type) {
        switch ( type ) {
        case Square:
            return createSquareMesh(name);
            break;
        case Cube:
            return createCubeMesh(name);
            break;
        case Sphere:
            return createSphereMesh(name);
            break;
        default:
            assert(("MESH::CREATEMESH::INVALIDTYPE") && false);
        }
        return nullptr;
    }

    std::shared_ptr<Mesh> Mesh::getShapeMesh(Type type) {
        if ( shapeMeshes.contains(type) )
            return shapeMeshes[type];

        std::string name;
        switch ( type ) {
        case Square:
            name = "SquareShape";
            break;
        case Cube:
            name = "CubeShape";
            break;
        case Sphere:
            name = "SphereShape";
            break;
        default:
            assert(("MESH::GETSHAPEMESH::INVALIDTYPE") && false);
            name = "InvalidShape";
        }

        shapeMeshes[type] = createMesh(name, type);
        return shapeMeshes[type];
    }

    void Mesh::clearBuffers() {
        loadedMeshes.clear();
        shapeMeshes.clear();
    }
} // namespace LaurelEye::Graphics
