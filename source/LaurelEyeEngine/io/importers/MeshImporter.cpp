#include "LaurelEyeEngine/io/importers/MeshImporter.h"
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/platforms/assimp/AssimpUtilities.h"

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
            const aiMesh* mesh = scene->mMeshes[m];
            populateVerticesIndices(meshAsset->vertices, meshAsset->indices, mesh, vertexOffset);

            // Update offset
            vertexOffset += mesh->mNumVertices;
        }

        return meshAsset;
    }

    std::shared_ptr<IAsset> MeshImporter::createSkinnedAsset(const std::string& path, const aiScene* scene) {
        unsigned int vertexOffset = 0;
        auto meshAsset = std::make_shared<SkinnedMeshAsset>(path);

        auto skeletonAsset = buildSkeleton(path, scene);
        const auto meshBoneCount = skeletonAsset->bones.size();
        meshAsset->skeleton = skeletonAsset;

        meshAsset->inverseBindMatrices.resize(meshBoneCount);

        for ( unsigned int m = 0; m < scene->mNumMeshes; ++m ) {
            aiMesh* mesh = scene->mMeshes[m];
            populateVerticesIndices(meshAsset->vertices, meshAsset->indices, mesh, vertexOffset);

            // Skin
            // NOTE: This should be combined with the skeleton stuff, cause the bone index matters!!
            for ( unsigned int i = 0; i < mesh->mNumBones; ++i ) {
                const aiBone* b = mesh->mBones[i];
                const std::string boneName = b->mName.C_Str();
                const auto it = skeletonAsset->boneNameIndex.find(boneName);
                assert("LAURELEYE::IO::MESHIMPORTER::BONEMISSING" && it != skeletonAsset->boneNameIndex.end());
                const auto boneIndex = it->second;

                for ( unsigned int j = 0; j < b->mNumWeights; ++j ) {
                    const aiVertexWeight& wt = b->mWeights[j];
                    int vertexId = vertexOffset + b->mWeights[j].mVertexId;

                    // AddBoneData function
                    addBoneData(meshAsset->vertices[vertexId], boneIndex, wt.mWeight);
                }

                meshAsset->inverseBindMatrices[boneIndex] = Platforms::Assimp::Utilities::convert(b->mOffsetMatrix);
            }

            // Update offset
            vertexOffset += mesh->mNumVertices;
        }

        return meshAsset;
    }

    void MeshImporter::buildSkeletonRecursive(const aiNode* node,
                                              int parentIndex,
                                              SkeletonAsset& skel,
                                              const std::unordered_set<std::string>& usedBoneNames) {
        std::string nodeName = node->mName.C_Str();
        int thisIndex = -1;

        if ( usedBoneNames.contains(nodeName) ) {
            thisIndex = static_cast<int>(skel.bones.size());

            SkeletonAsset::Bone bone;
            bone.name = nodeName;
            bone.localBindTransform = Platforms::Assimp::Utilities::convert(node->mTransformation); // parent → bone in bind pose

            skel.bones.push_back(bone);
            skel.parentIndices.push_back(parentIndex);
            skel.boneNameIndex[bone.name] = thisIndex;

            parentIndex = thisIndex; // children who are also bones will use this as parent
        }

        for ( unsigned int c = 0; c < node->mNumChildren; ++c ) {
            buildSkeletonRecursive(node->mChildren[c], parentIndex, skel, usedBoneNames);
        }
    }

    std::shared_ptr<SkeletonAsset> MeshImporter::buildSkeleton(const std::string& path, const aiScene* scene) {
        std::unordered_set<std::string> usedBoneNames;

        for ( unsigned int m = 0; m < scene->mNumMeshes; ++m ) {
            aiMesh* mesh = scene->mMeshes[m];
            for ( unsigned int b = 0; b < mesh->mNumBones; ++b ) {
                aiBone* bone = mesh->mBones[b];
                usedBoneNames.insert(bone->mName.C_Str());
            }
        }

        auto asset = std::make_shared<SkeletonAsset>(path);

        buildSkeletonRecursive(scene->mRootNode, -1, *asset.get(), usedBoneNames);

        return asset;
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
    template <std::convertible_to<LaurelEye::Graphics::MeshVertex> TVertex>
    void MeshImporter::populateVerticesIndices(std::vector<TVertex>& vertices,
                                               std::vector<uint32_t>& indices,
                                               const aiMesh* mesh,
                                               uint32_t vertexOffset) {
        // Expand vertex array
        size_t baseIndex = vertices.size();
        vertices.resize(baseIndex + mesh->mNumVertices);

        for ( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
            auto& v = vertices[baseIndex + i];

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
                indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }
    }

} // namespace LaurelEye::IO
