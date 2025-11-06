/////////////////////////////////////////////////////////////////////////
// Vertex shader for Debug Draw
//
// For LaurelEyeEngine
////////////////////////////////////////////////////////////////////////
#version 430 core

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

layout(location = 0) in vec3 vertex;
layout(location = 3) in vec4 color;

out vec4 vColor;

void main()
{

    vColor = color;
    gl_Position = projection * view * vec4(vertex, 1.0);

}
