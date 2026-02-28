#version 450

in vec4 position;      // need to pass this from vertex shader

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float dist = length(position.xyz - lightPos);
    gl_FragDepth = dist / farPlane;
}
