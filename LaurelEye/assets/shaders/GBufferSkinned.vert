///////////////////////////////////////////////////////////////////////////////
// Vertex shader for GBuffer
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 440

const int MAX_BONE_INFLUENCE = 4;

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;
layout(location = 4) in vec3 vertexTangent;
layout(location = 4) in ivec4 boneIndices;
layout(location = 4) in vec4 boneWeights;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

layout(std430, binding = 3) readonly buffer SkinData {
    uint boneCount;
    mat4 inverseBind[];
};

layout (binding = 4) uniform animationData {
    mat4 globalPose[];
};

uniform mat4 ModelMatrix, InvModelMatrix;

out vec3 eyeVec;
out vec3 normalVec, worldPos;
out vec2 texCoord;
out vec3 tanVec;

uniform bool isSkinned;
uniform bool isSkydome;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    vec3 totalTangent = vec3(0.0f);
    if ( !isSkinned ) {
            totalPosition = vertex;
            totalNormal = vertexNormal;
            totalTangent = vertexTangent;
    }
    else {
        for ( int i = 0; i < MAX_BONE_INFLUENCE; i++ ) {
            if ( boneIndices[i] == -1 )
                continue;
            if ( boneIndices[i] >= skinData.boneCount ) {
                totalPosition = vertex;
                totalNormal = vertexNormal;
                totalTangent = vertexTangent;
                break;
            }
            vec4 localPosition = animationData.globalPose[boneIndices[i]] * skinData.inverseBind[boneIndices[i]] * vertex;
            totalPosition += localPosition * boneWeights[i];
            vec3 localNormal = mat3(animationData.globalPose[boneIndices[i]]) * vertexNormal;
            totalNormal += localNormal * boneWeights[i];
            vec3 localTangent = mat3(animationData.globalPose[boneIndices[i]]) * vertexTangent;
            totalTangent += localTangent * boneWeights[i];
        }
    }


    // Computes world position at a pixel used for
    // light and eye vector calculations
    worldPos = (ModelMatrix * totalPosition).xyz;

    // Computes the points projection on the screen
    gl_Position = projection * view * ModelMatrix * totalPosition;

    normalVec = totalNormal * mat3(InvModelMatrix);

    tanVec = mat3(ModelMatrix) * totalTangent;

    texCoord = vertexTexture;

    mat4 rotView = mat4(mat3(view));
    vec4 pos = rotView * totalPosition;
    eyeVec = -(totalPosition).xyz;

    if(isSkydome) gl_Position = projection * pos;
}
