///////////////////////////////////////////////////////////////////////////////
// Vertex shader for UI
///////////////////////////////////////////////////////////////////////////////
#version 440

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

uniform mat4 ModelMatrix;

out vec2 texCoord;

void main()
{

    // Computes the point projection on the screen
    gl_Position = ModelMatrix*vertex;

    texCoord = vertexTexture;
}
