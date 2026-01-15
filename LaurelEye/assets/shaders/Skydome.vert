///////////////////////////////////////////////////////////////////////////////
// Vertex shader for Skydome
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 430

in vec4 vertex;
in vec3 vertexNormal;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

out vec3 eyePos;

void main()
{
    // Strip translation from view to keep sky centered around camera
    mat4 rotView = mat4(mat3(view));

    vec4 pos = rotView * vertex;

    eyePos = -(vertex).xyz;

    // Computes the point’s projection on the screen
    gl_Position = projection * pos;
}
