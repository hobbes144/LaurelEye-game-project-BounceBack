/// @file   Assets.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-30-2025
/// @brief  A series of Asset type definitions. Includes the following:
/// Images, 3D Meshes, JSON, and text
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
#include "LaurelEyeEngine/io/IAsset.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Transform.h"

#include <cstdint>
#include <memory>
#include <rapidjson/document.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace LaurelEye::IO {

    struct ImageAsset : public IAsset {
        using IAsset::IAsset; // Inherits default constructor

        enum PixelFormat {
            UNKNOWN,
            RGB8,
            RGBA8,
            HDR_FLOAT
        };

        int width = 0, height = 0, channels = 0;
        PixelFormat format = PixelFormat::UNKNOWN;
        std::vector<unsigned char> pixelData; // raw RGBA, etc.
    };

    struct JsonAsset : public IAsset {
        using IAsset::IAsset; // Inherits default constructor
        rapidjson::Document jsonDocument;
    };

    struct TextAsset : public IAsset {
        using IAsset::IAsset; // Inherits default constructor
        std::string text;
    };

    struct ShaderAsset : public IAsset {
        using IAsset::IAsset; // Inherits default constructor
        enum class Stage {
            Unknown,
            Vertex,
            Fragment,
            Geometry,
            TessControl,
            TessEval,
            Compute
        };

        std::unordered_map<Stage, std::string> sources;

        // Optional: entry points for Vulkan
        // std::unordered_map<Stage, std::string> entryPoints;
    };

    /// @class AnimationAsset
    /// @brief Animation info
    ///
    /// Each Channel stores one bone's animation keyframes.
    /// Channels MUST be in the same vector order as the bones in the
    /// SkeletonAsset and the MeshAsset.
    ///
    /// In the final implementation, ideally this should be loaded along with
    /// the MeshAsset and SkeletonAsset in an AssetImporter that is specific to
    /// files read by Assimp instead of something called a MeshImporter.
    ///
    struct AnimationAsset : public IAsset {
        using IAsset::IAsset;

        /// @class Channel
        /// @brief Each channel holds one bone node's animation keyframes
        ///
        /// The number of Position, Rotation and Scale values will be equal.
        /// Use length of any one of them to size the Transform and double
        /// vectors.
        struct Channel {
            std::vector<Transform> keyframe;
            std::vector<double> timeStamp;
        };

        std::vector<Channel> channels;
        std::string animName;
        double ticksPerSecond;
        double duration;

        void reserveSize(size_t boneCount) {
            channels.resize(boneCount);
        }
    };

    struct SkeletonAsset : public IAsset {
        using IAsset::IAsset;

        struct Bone {
            std::string name;
            /// @brief Local bind pose transformation relative to parent bone.
            Transform localBindTransform;
        };

        // Skeleton name is typically NOT the path name. Need to handle that later.
        // std::string name;
        std::vector<Bone> bones;
        std::vector<int> parentIndices;
        Transform inverseTransform;
        std::unordered_map<std::string, int> boneNameIndex;
    };

    struct MeshAsset : public IAsset {
        using IAsset::IAsset; // Inherits default constructor
        using Vertex = Graphics::MeshVertex;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        int materialIndex = -1; // maps to MaterialAsset later
    };

    /// @class SkinnedMeshAsset
    /// @brief Skinned version of Mesh, which holds a skin and ref to a
    /// skeleton.
    ///
    struct SkinnedMeshAsset : public MeshAsset {
        using MeshAsset::MeshAsset; // Inherits default constructor
        using SkinnedVertex = Graphics::SkinnedMeshVertex;

        std::vector<SkinnedVertex> vertices;
        std::vector<uint32_t> indices;

        int materialIndex = -1; // maps to MaterialAsset later

        /// @brief SkeletonAsset that ties to this mesh.
        ///
        /// We will store the skeleton in this Mesh type for now.
        /// Ideally this should be initialized separately from the mesh so we
        /// can reuse the skeleton over multiple meshes, which can each have
        /// different properties.
        std::shared_ptr<SkeletonAsset> skeleton;

        /// @brief AnimationAsset that ties to this mesh.
        ///
        /// Ideally this should be completely separated and sent to the
        /// AnimationSystem, but no time. For now, AnimationSystem is under
        /// RenderSystem and we will read this and set up AnimationSystem
        /// there.
        std::shared_ptr<AnimationAsset> animation;

        /// @brief Inverse bind transforms.
        ///
        /// This is used by the mesh's skin to manage vertex weights.
        /// It is in the order of the bone index. This ensures that we can
        /// easily access the relevant bone's inverse bind in the same index
        /// space as the animation transforms.
        std::vector<Matrix4> inverseBindMatrices;
    };

    struct SceneAsset : public IAsset {
        using IAsset::IAsset; // Inherits default constructor
        struct Node {
            std::string name;
            std::vector<std::shared_ptr<MeshAsset>> meshes;
            std::vector<std::shared_ptr<ImageAsset>> images;
            // Animation links could go here
            std::vector<std::shared_ptr<Node>> children;
        };

        std::vector<std::shared_ptr<MeshAsset>> meshes;
        std::vector<std::shared_ptr<ImageAsset>> images;
        // Skeletal animations, cameras, lights etc.
        // Assimp supports them, add when needed.
        std::shared_ptr<Node> root;
    };

} // namespace LaurelEye::IO
