///////////////////////////////////////////////////////////////////////////////
// Vertex shader for GBuffer
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 440

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;
layout(location = 4) in vec3 vertexTangent;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

uniform mat4 ModelMatrix, InvModelMatrix;

out vec3 eyeVec;
out vec3 normalVec, worldPos;
out vec2 texCoord;
out vec3 tanVec;

uniform bool isSkydome;

void main()
{
    // Computes world position at a pixel used for
    // light and eye vector calculations
    worldPos=(ModelMatrix*vertex).xyz;

    // Computes the points projection on the screen
    gl_Position = projection*view*ModelMatrix*vertex;

    normalVec = vertexNormal*mat3(InvModelMatrix);

    tanVec = mat3(ModelMatrix)*vertexTangent;

    texCoord = vertexTexture;

    mat4 rotView = mat4(mat3(view));
    vec4 pos = rotView * vertex;
    eyeVec = -(vertex).xyz;

    if(isSkydome) gl_Position = projection * pos;
}
