/////////////////////////////////////////////////////////////////////////
// Vertex shader for lighting
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 3) in vec4 vertexColor;
layout(location = 6) in float inSize;
layout(location = 7) in float inRotation;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

out vec4 vColor;
out float vRotation;


void main()
{
    gl_Position = projection * view * vec4(vertex, 1.0);
    gl_PointSize = inSize;   // dynamic size per particle
    vColor = vertexColor;
    vRotation = inRotation;  // pass to fragment shader

    vColor = vertexColor;
}
