#include "LaurelEyeEngine/io/importers/MeshImporter.h"
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/math/Transform.h"
#include "LaurelEyeEngine/platforms/assimp/AssimpUtilities.h"

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <memory>

namespace LaurelEye::IO {
    std::shared_ptr<IAsset> MeshImporter::import(const std::string& path) {
        Assimp::Importer importer;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
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

        auto animationAsset = buildAnimation(path, scene, *skeletonAsset);
        // TODO - can be adapted to a vector of animation assets later
        meshAsset->animation = animationAsset;

        // At this point:
        // Skeleton's bone order has been established, so you need to set the
        // order of inverseBindMatrices and animation channel Transform vectors
        // to follow that order. Read the bone name and position in the
        // Skeleton's Bone vector, and fill that index in the target vector.

        // Add animation import code here.

        meshAsset->inverseBindMatrices.resize(meshBoneCount);

        // for ( unsigned int m = 0; m < scene->mNumMeshes; ++m ) {
        aiMesh* mesh = scene->mMeshes[0];
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
            // }

            // Update offset
            // vertexOffset += mesh->mNumVertices;
        }

        return meshAsset;
    }

    void MeshImporter::buildSkeletonRecursive(const aiNode* node,
                                              int parentIndex,
                                              SkeletonAsset& skel,
                                              Transform accumulated,
                                              std::unordered_set<std::string>& usedBoneNames) {
        std::string nodeName = node->mName.C_Str();
        int thisIndex = parentIndex;

        if ( usedBoneNames.contains(nodeName) ) {
            assert(!skel.boneNameIndex.contains(nodeName) && "ERROR::LAURELEYE::MESHIMPORTER::INVALID_SKELETON");
            thisIndex = static_cast<int>(skel.bones.size());

            SkeletonAsset::Bone bone;
            bone.name = nodeName;
            bone.localBindTransform = accumulated * Platforms::Assimp::Utilities::convertTransform(node->mTransformation); // parent → bone in bind pose
            accumulated = Transform();

            skel.boneNameIndex[nodeName] = thisIndex;

            skel.bones.push_back(bone);
            skel.parentIndices.push_back(parentIndex);
        }
        else {
            accumulated = accumulated * Platforms::Assimp::Utilities::convertTransform(node->mTransformation);
        }
        parentIndex = thisIndex; // children who are also bones will use this as parent

        for ( unsigned int c = 0; c < node->mNumChildren; ++c ) {
            buildSkeletonRecursive(node->mChildren[c], parentIndex, skel, accumulated, usedBoneNames);
        }
    }

    std::shared_ptr<SkeletonAsset> MeshImporter::buildSkeleton(const std::string& path, const aiScene* scene) {
        std::unordered_set<std::string> usedBoneNames;
        int thisIndex = -1;
        auto asset = std::make_shared<SkeletonAsset>(path);
        // unused, remove:
        asset->inverseTransform = Platforms::Assimp::Utilities::convert(scene->mRootNode->mTransformation.Inverse());

        // First get the list of actual bones.
        aiMesh* mesh = scene->mMeshes[0];
        for ( unsigned int b = 0; b < mesh->mNumBones; ++b ) {
            aiBone* bone = mesh->mBones[b];
            if ( !usedBoneNames.contains(bone->mName.C_Str()) ) {
                usedBoneNames.insert(bone->mName.C_Str());
            }
        }

        // Assimp's trees have way more nodes than just the bones, and every
        // node's transform is important. To handle this, we use an
        // accumulator that adds up the bindTransforms of each of the nodes
        // between the bones.

        Transform accumulatedBindTransform = Transform();

        buildSkeletonRecursive(scene->mRootNode, -1, *asset.get(), accumulatedBindTransform, usedBoneNames);

        return asset;
    }

    std::string CleanName(const std::string& name) {
        const std::string suffix = "_$AssimpFbx$_";
        size_t pos = name.find(suffix);
        if ( pos != std::string::npos )
            return name.substr(0, pos);
        return name;
    }

    std::shared_ptr<AnimationAsset> MeshImporter::buildAnimation(const std::string& path, const aiScene* scene, const SkeletonAsset& skeleton) {
        if ( scene->mNumAnimations == 0 )
            return nullptr;

        // First anim for now
        aiAnimation* aiAnim = scene->mAnimations[0];

        auto anim = std::make_shared<AnimationAsset>(path);
        anim->animName = extractName(path);
        anim->duration = aiAnim->mDuration;
        anim->ticksPerSecond = aiAnim->mTicksPerSecond != 0.0 ? aiAnim->mTicksPerSecond : 30.0;

        // Skeleton order = animation channel index order, match here
        size_t boneCount = skeleton.bones.size();
        anim->reserveSize(boneCount);

        // maps animation channels to skeleton bone indices
        for ( size_t c = 0; c < aiAnim->mNumChannels; ++c ) {
            aiNodeAnim* aiChannel = aiAnim->mChannels[c];
            std::string boneName = aiChannel->mNodeName.C_Str();
            boneName = CleanName(boneName);

            // Find matching bone index in skeleton if it exists
            auto it = skeleton.boneNameIndex.find(boneName);
            if ( it == skeleton.boneNameIndex.end() ) {
                std::cout << "Animation channel bone not found in skeleton: " << boneName << std::endl;
                continue;
            }

            size_t boneIndex = it->second;
            AnimationAsset::Channel& ch = anim->channels[boneIndex];

            // Determine number of keys
            size_t keyCount = std::max({aiChannel->mNumPositionKeys,
                                        aiChannel->mNumRotationKeys,
                                        aiChannel->mNumScalingKeys});

            ch.keyframe.reserve(keyCount);
            ch.timeStamp.reserve(keyCount);

            for ( size_t i = 0; i < keyCount; ++i ) {
                Transform t;

                // POSITION
                if ( i < aiChannel->mNumPositionKeys ) {
                    const auto& p = aiChannel->mPositionKeys[i].mValue;
                    t.setPosition(p.x, p.y, p.z);
                }
                else t.setPosition(0, 0, 0);

                // ROTATION
                if ( i < aiChannel->mNumRotationKeys ) {
                    const auto& r = aiChannel->mRotationKeys[i].mValue;
                    t.setRotation(Quaternion(r.w, r.x, r.y, r.z));
                }
                else t.setRotation(Quaternion());

                // SCALE
                if ( i < aiChannel->mNumScalingKeys ) {
                    const auto& s = aiChannel->mScalingKeys[i].mValue;
                    t.setScaling(s.x, s.y, s.z);
                }
                else t.setScaling(1, 1, 1);

                ch.keyframe.push_back(t);

                // determine timestamp from the keys with the highest time
                double tPos = (i < aiChannel->mNumPositionKeys)
                                ? aiChannel->mPositionKeys[i].mTime
                                : 0.0;
                double tRot = (i < aiChannel->mNumRotationKeys)
                                ? aiChannel->mRotationKeys[i].mTime
                                : 0.0;
                double tScale = (i < aiChannel->mNumScalingKeys)
                                  ? aiChannel->mScalingKeys[i].mTime
                                  : 0.0;

                double tstamp = std::max({tPos, tRot, tScale});
                ch.timeStamp.push_back(tstamp);
            }
        }

        // After processing all aiAnim->mChannels
        for ( size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex ) {
            AnimationAsset::Channel& ch = anim->channels[boneIndex];

            // If this bone already has animation keys, skip it
            if ( !ch.keyframe.empty() )
                continue;

            // Bone has no animation channel — use bind transform
            const Transform& bindT = skeleton.bones[boneIndex].localBindTransform;

            ch.keyframe.push_back(bindT);
            ch.timeStamp.push_back(0.0);

            std::cout << "Added bind-pose channel for missing bone: "
                      << skeleton.bones[boneIndex].name << std::endl;
        }

        return anim;


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
