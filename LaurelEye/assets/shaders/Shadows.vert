///////////////////////////////////////////////////////////////////////////////
// Vertex shader for Shadow
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 450

in vec4 vertex;
in vec3 vertexNormal;
in vec2 vertexTexture;
in vec3 vertexTangent;

layout (binding = 0) uniform camera
{
    mat4 ProjViewMatrix;
};

uniform mat4 ModelMatrix;

out vec4 position;

void main()
{
    // Computes the points projection on the screen
    gl_Position = ProjViewMatrix*ModelMatrix*vertex;
    position = gl_Position;
}
