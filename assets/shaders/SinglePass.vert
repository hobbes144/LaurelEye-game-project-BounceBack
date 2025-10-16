/////////////////////////////////////////////////////////////////////////
// Vertex shader for lighting
//
// For LaurelEyeEngine
////////////////////////////////////////////////////////////////////////
#version 430 core

uniform mat4 WorldView, WorldInverse, WorldProj, ModelMatrix, InvModelMatrix;

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;
layout(location = 3) in vec3 vertexTangent;

out vec3 normalVec, lightVec, eyeVec;
out vec2 texCoord;
uniform vec3 lightPos;

void main()
{
    // Computes world position at a pixel used for
    // light and eye vector calculations
    vec3 worldPos;
    worldPos=(ModelMatrix*vertex).xyz;

    // Computes the point�s projection on the screen
    gl_Position = WorldProj*WorldView*ModelMatrix*vertex;

    normalVec = vertexNormal*mat3(InvModelMatrix);
    vec3 eyePos = (WorldInverse*vec4(0,0,0,1)).xyz;
    lightVec = lightPos - worldPos;
    eyeVec = eyePos-worldPos;

    texCoord = vertexTexture;
}
