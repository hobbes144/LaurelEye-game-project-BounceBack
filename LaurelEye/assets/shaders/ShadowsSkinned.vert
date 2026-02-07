///////////////////////////////////////////////////////////////////////////////
// Vertex shader for Shadow
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 450

const int MAX_BONE_INFLUENCE = 4;
const uint UINT_MAX = 0xFFFFFFFFu; // or 4294967295U;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;
layout(location = 4) in vec3 vertexTangent;
layout(location = 8) in ivec4 boneIndices;
layout(location = 9) in vec4 boneWeights;

layout (binding = 0) uniform camera
{
    mat4 ProjViewMatrix;
};

layout(std430, binding = 3) readonly buffer SkinData {
    int boneCount;
    mat4 inverseBind[];
} skinData;

layout(std430, binding = 4) readonly buffer AnimationData {
    mat4 globalPose[];
} animationData;

uniform mat4 ModelMatrix;

out vec4 position;
uniform bool isSkinned;

void main()
{
    // Computes the points projection on the screen
    vec4 totalPosition = vec4(0.0f);
    if (!isSkinned) {
        totalPosition = vec4(vertex, 1.0);
    }
    else
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if (boneIndices[i] == -1)
                continue;
            if (boneIndices[i] >= skinData.boneCount)
            {
                totalPosition = vec4(vertex, 1.0); break;
            }
            const mat4 M = animationData.globalPose[boneIndices[i]] * skinData.inverseBind[boneIndices[i]];
            const vec4 localPosition = M * vec4(vertex, 1.0); totalPosition += localPosition * boneWeights[i];
        }
    }
    vec4 worldPos = ModelMatrix * totalPosition;
    gl_Position = ProjViewMatrix * worldPos;
    position = gl_Position;
}
