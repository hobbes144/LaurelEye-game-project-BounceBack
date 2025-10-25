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
#include <rapidjson/document.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "LaurelEyeEngine/io/IAsset.h"

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

    struct MeshAsset : public IAsset {
        using IAsset::IAsset; // Inherits default constructor
        struct Vertex {
            float position[3];
            float normal[3];
            float uv[2];
            float tangent[3];
            float bitangent[3];
            // int boneIndices[4];
            // float boneWeights[4];
        };

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        int materialIndex = -1; // maps to MaterialAsset later
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
