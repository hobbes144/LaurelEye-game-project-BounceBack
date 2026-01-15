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

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

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

    std::shared_ptr<Mesh> Mesh::loadMesh(const std::string& filename) {
        // if already have this file, return the cached mesh.
        if ( loadedMeshes.find(filename) != loadedMeshes.end() )
            return loadedMeshes[filename];

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filename,
                                                 aiProcess_Triangulate |
                                                     aiProcess_FlipUVs |
                                                     aiProcess_GenNormals |
                                                     aiProcess_CalcTangentSpace);

        if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
            std::cerr << "Assimp error loading " << filename << ": "
                      << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        std::vector<Attributes> meshesAttributes;
        std::vector<unsigned int> combinedIndices;
        unsigned int vertexOffset = 0;

        for ( unsigned int i = 0; i < scene->mNumMeshes; i++ ) {
            aiMesh* aMesh = scene->mMeshes[i];
            Attributes tempAttributes;
            tempAttributes[GeometryBuffer::AttributeType::Position] = std::make_pair(std::vector<float>(), 3);
            tempAttributes[GeometryBuffer::AttributeType::Normal] = std::make_pair(std::vector<float>(), 3);
            tempAttributes[GeometryBuffer::AttributeType::TexCoord] = std::make_pair(std::vector<float>(), 2);
            tempAttributes[GeometryBuffer::AttributeType::Tangent] = std::make_pair(std::vector<float>(), 3);

            std::vector<unsigned int> meshIndices;
            // process the individual mesh.
            processMesh(aMesh, tempAttributes, meshIndices);

            // adjust the indices from this mesh by the current vertex offset.
            combineIndices(vertexOffset, combinedIndices, meshIndices);

            // Update vertexOffset
            size_t numVertices = tempAttributes[GeometryBuffer::AttributeType::Position].first.size() / 3;
            vertexOffset += static_cast<unsigned int>(numVertices);

            meshesAttributes.push_back(tempAttributes);
        }

        // combine the per‑mesh attribute data into one Attributes object.
        Attributes combinedAttributes = combineAttributes(meshesAttributes);

        // create the new mesh instance using the combined attributes and indices.
        std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(filename + "_Mesh", combinedAttributes, combinedIndices, 0);

        // cache the loaded mesh.
        loadedMeshes[filename] = newMesh;
        return newMesh;
    }

    void Mesh::processMesh(aiMesh* mesh, Attributes& newMeshData, std::vector<unsigned int>& indices) {
        // Reserve space based on the number of vertices.
        newMeshData[GeometryBuffer::AttributeType::Position].first.reserve(mesh->mNumVertices * 3);
        newMeshData[GeometryBuffer::AttributeType::Normal].first.reserve(mesh->mNumVertices * 3);
        newMeshData[GeometryBuffer::AttributeType::TexCoord].first.reserve(mesh->mNumVertices * 2);
        newMeshData[GeometryBuffer::AttributeType::Tangent].first.reserve(mesh->mNumVertices * 3);

        // Process vertices.
        for ( unsigned int i = 0; i < mesh->mNumVertices; i++ ) {
            // Position.
            newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(mesh->mVertices[i].x);
            newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(mesh->mVertices[i].y);
            newMeshData[GeometryBuffer::AttributeType::Position].first.push_back(mesh->mVertices[i].z);

            // Normal.
            if ( mesh->HasNormals() ) {
                newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(mesh->mNormals[i].x);
                newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(mesh->mNormals[i].y);
                newMeshData[GeometryBuffer::AttributeType::Normal].first.push_back(mesh->mNormals[i].z);
            }

            // Texture Coordinates (first set only).
            if ( mesh->HasTextureCoords(0) ) {
                newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(mesh->mTextureCoords[0][i].x);
                newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(0.0f);
                newMeshData[GeometryBuffer::AttributeType::TexCoord].first.push_back(0.0f);
            }

            // Tangent.
            if ( mesh->HasTangentsAndBitangents() ) {
                newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(mesh->mTangents[i].x);
                newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(mesh->mTangents[i].y);
                newMeshData[GeometryBuffer::AttributeType::Tangent].first.push_back(mesh->mTangents[i].z);
            }
        }

        // Process indices.
        for ( unsigned int i = 0; i < mesh->mNumFaces; i++ ) {
            aiFace face = mesh->mFaces[i];
            for ( unsigned int j = 0; j < face.mNumIndices; j++ ) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

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

    std::shared_ptr<Mesh> Mesh::createMeshFromAsset(const std::shared_ptr<IO::MeshAsset>& meshAsset) {
        if ( !meshAsset || meshAsset->vertices.empty() ) {
            throw("Mesh::FromAsset - Invalid or empty MeshAsset.");
            return nullptr;
        }

        // Reuse loaded mesh if already present.
        if ( loadedMeshes.contains(meshAsset->getPath()) )
            return loadedMeshes[meshAsset->getPath()];

        Mesh::Attributes attributes;

        const size_t vertexCount = meshAsset->vertices.size();
        std::vector<float> positions, normals, uvs, tangents, bitangents;

        positions.reserve(vertexCount * 3);
        normals.reserve(vertexCount * 3);
        uvs.reserve(vertexCount * 2);
        tangents.reserve(vertexCount * 3);
        bitangents.reserve(vertexCount * 3);

        assert("MESH::CREATE_MESH_FROM_ASSET::DEPRICATED" && false);
        // for ( const auto& v : meshAsset->vertices ) {
        //     positions.insert(positions.end(), v.position, v.position + 3);
        //     normals.insert(normals.end(), v.normal, v.normal + 3);
        //     uvs.insert(uvs.end(), v.uv, v.uv + 2);
        //     tangents.insert(tangents.end(), v.tangent, v.tangent + 3);
        //     // bitangents.insert(bitangents.end(), v.bitangent, v.bitangent + 3);
        // }

        attributes[GeometryBuffer::AttributeType::Position] = {positions, 3};
        attributes[GeometryBuffer::AttributeType::Normal] = {normals, 3};
        attributes[GeometryBuffer::AttributeType::TexCoord] = {uvs, 2};
        attributes[GeometryBuffer::AttributeType::Tangent] = {tangents, 3};
        attributes[GeometryBuffer::AttributeType::Bitangent] = {bitangents, 3};

        std::shared_ptr<Mesh> mesh;
        if ( !meshAsset->indices.empty() )
            mesh = std::make_shared<Mesh>(meshAsset->getName(), attributes, meshAsset->indices);
        else
            mesh = std::make_shared<Mesh>(meshAsset->getName(), attributes);

        loadedMeshes[meshAsset->getPath()] = mesh;
        return mesh;
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
