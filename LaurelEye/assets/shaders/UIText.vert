#version 440
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUV;  // ← was 1, should be 2 to match TexCoord slot
uniform mat4 m_Model;
uniform vec2 u_ScreenSize;
uniform vec2 u_UV0;
uniform vec2 u_UV1;
out vec2 vUV;
void main()
{
    vec4 world = m_Model * vec4(aPos, 1.0);
    vec2 ndc;
    ndc.x = (world.x / u_ScreenSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (world.y / u_ScreenSize.y) * 2.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    vUV = mix(u_UV0, u_UV1, aUV);
}
