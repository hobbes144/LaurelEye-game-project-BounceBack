#include "LaurelEyeEngine/io/importers/MeshImporter.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <memory>

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> MeshImporter::import(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
                                                 aiProcess_CalcTangentSpace |
                                                     aiProcess_JoinIdenticalVertices |
                                                     aiProcess_Triangulate |
                                                     aiProcess_GenSmoothNormals |
                                                     aiProcess_ImproveCacheLocality |
                                                     aiProcess_FixInfacingNormals |
                                                     aiProcess_GenUVCoords |
                                                     aiProcess_OptimizeMeshes |
                                                     aiProcess_OptimizeGraph);

        if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
            throw std::runtime_error("MeshImporter: Failed to load mesh: " + path +
                                     " (" + importer.GetErrorString() + ")");
            return nullptr;
        }

        if ( scene->HasMeshes() && scene->mMeshes[0]->HasBones() ) {
            return createSkinnedAsset(path, scene);
        }
        else {
            return createAsset(path, scene);
        }
    }

    std::shared_ptr<IAsset> MeshImporter::createAsset(const std::string& path, const aiScene* scene) {
        unsigned int vertexOffset = 0;
        auto meshAsset = std::make_shared<MeshAsset>(path);
        for ( unsigned int m = 0; m < scene->mNumMeshes; ++m ) {
            aiMesh* mesh = scene->mMeshes[m];

            // Expand vertex array
            size_t baseIndex = meshAsset->vertices.size();
            meshAsset->vertices.resize(baseIndex + mesh->mNumVertices);

            for ( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
                auto& v = meshAsset->vertices[baseIndex + i];

                // Position
                v.position[0] = mesh->mVertices[i].x;
                v.position[1] = mesh->mVertices[i].y;
                v.position[2] = mesh->mVertices[i].z;

                // Normals
                if ( mesh->HasNormals() ) {
                    v.normal[0] = mesh->mNormals[i].x;
                    v.normal[1] = mesh->mNormals[i].y;
                    v.normal[2] = mesh->mNormals[i].z;
                }

                // Texture coordinates
                if ( mesh->HasTextureCoords(0) ) {
                    v.uv[0] = mesh->mTextureCoords[0][i].x;
                    v.uv[1] = mesh->mTextureCoords[0][i].y;
                }

                // Tangents
                if ( mesh->HasTangentsAndBitangents() ) {
                    v.tangent[0] = mesh->mTangents[i].x;
                    v.tangent[1] = mesh->mTangents[i].y;
                    v.tangent[2] = mesh->mTangents[i].z;
                }
            }

            // Indices
            for ( unsigned int f = 0; f < mesh->mNumFaces; ++f ) {
                const aiFace& face = mesh->mFaces[f];
                for ( unsigned int j = 0; j < face.mNumIndices; ++j ) {
                    meshAsset->indices.push_back(face.mIndices[j] + vertexOffset);
                }
            }

            // Update offset
            vertexOffset += mesh->mNumVertices;
        }

        return meshAsset;
    }

    std::shared_ptr<IAsset> MeshImporter::createSkinnedAsset(const std::string& path, const aiScene* scene) {
        unsigned int vertexOffset = 0;
        auto meshAsset = std::make_shared<SkinnedMeshAsset>(path);

        for ( unsigned int m = 0; m < scene->mNumMeshes; ++m ) {
            aiMesh* mesh = scene->mMeshes[m];

            // Expand vertex array
            size_t baseIndex = meshAsset->vertices.size();
            meshAsset->vertices.resize(baseIndex + mesh->mNumVertices);

            for ( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
                auto& v = meshAsset->vertices[baseIndex + i];

                // Position
                v.position[0] = mesh->mVertices[i].x;
                v.position[1] = mesh->mVertices[i].y;
                v.position[2] = mesh->mVertices[i].z;

                // Normals
                if ( mesh->HasNormals() ) {
                    v.normal[0] = mesh->mNormals[i].x;
                    v.normal[1] = mesh->mNormals[i].y;
                    v.normal[2] = mesh->mNormals[i].z;
                }

                // Texture coordinates
                if ( mesh->HasTextureCoords(0) ) {
                    v.uv[0] = mesh->mTextureCoords[0][i].x;
                    v.uv[1] = mesh->mTextureCoords[0][i].y;
                }

                // Tangents
                if ( mesh->HasTangentsAndBitangents() ) {
                    v.tangent[0] = mesh->mTangents[i].x;
                    v.tangent[1] = mesh->mTangents[i].y;
                    v.tangent[2] = mesh->mTangents[i].z;
                }
            }

            // Skin
            // NOTE: This should be combined with the skeleton stuff, cause the bone index matters!!
            for ( unsigned int i = 0; i < mesh->mNumBones; ++i ) {
                const aiBone* b = mesh->mBones[i];

                for ( unsigned int j = 0; j < b->mNumWeights; ++j ) {
                    const aiVertexWeight& wt = b->mWeights[j];
                    int vertexId = vertexOffset + b->mWeights[j].mVertexId;

                    // AddBoneData function
                    addBoneData(meshAsset->vertices[vertexId], i, wt.mWeight);
                }
            }

            // Indices
            for ( unsigned int f = 0; f < mesh->mNumFaces; ++f ) {
                const aiFace& face = mesh->mFaces[f];
                for ( unsigned int j = 0; j < face.mNumIndices; ++j ) {
                    meshAsset->indices.push_back(face.mIndices[j] + vertexOffset);
                }
            }

            // Update offset
            vertexOffset += mesh->mNumVertices;
        }

        return meshAsset;
    }

    void MeshImporter::addBoneData(SkinnedMeshAsset::SkinnedVertex& vertex, int boneIndex, float boneWeight) {
        int arr_size = sizeof(vertex.boneIndices) / sizeof(vertex.boneIndices[0]);
        for ( int i = 0; i < arr_size; i++ ) {
            if ( vertex.boneWeights[i] == 0.0 ) {
                vertex.boneIndices[i] = boneIndex;
                vertex.boneWeights[i] = boneWeight;
                break;
            }
        }
    }

} // namespace LaurelEye::IO
