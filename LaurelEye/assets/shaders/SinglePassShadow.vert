/////////////////////////////////////////////////////////////////////////
// Vertex shader for lighting
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 430 core

uniform mat4 ModelMatrix, InvModelMatrix;

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;
layout(location = 3) in vec3 vertexTangent;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

out vec3 normalVec, worldPos;
out vec2 texCoord;
out vec3 tanVec;

void main()
{
    worldPos=(ModelMatrix*vertex).xyz;

    // Computes the point�s projection on the screen
    gl_Position = projection*view*ModelMatrix*vertex;

    normalVec = vertexNormal*mat3(InvModelMatrix);

    tanVec = mat3(ModelMatrix)*vertexTangent;

    texCoord = vertexTexture;

}
