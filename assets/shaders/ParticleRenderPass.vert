/////////////////////////////////////////////////////////////////////////
// Vertex shader for lighting
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 vertexColor;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

out vec4 vColor;

void main()
{
    // Computes the point�s projection on the screen
    gl_Position = projection*view*vec4(vertex,1.0);
    gl_PointSize = 8.0; // pixel size of each particle

    vColor = vertexColor;
}
