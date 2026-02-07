///////////////////////////////////////////////////////////////////////////////
// Vertex shader for UI
///////////////////////////////////////////////////////////////////////////////
#version 440

layout(location = 0) in vec4 vertex;
layout(location = 2) in vec2 vertexTexture;

uniform mat4 m_Model;
uniform vec2 u_ScreenSize; // framebuffer size

out vec2 texCoord;

void main()
{

    vec4 world = m_Model * vertex;

    vec2 ndc = (world.xy / u_ScreenSize) * 2.0 - 1.0;

    gl_Position = vec4(ndc, 0.0, 1.0);

    texCoord = vertexTexture;

}
