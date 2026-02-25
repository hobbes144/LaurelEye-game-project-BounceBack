#version 450

in vec4 position;      // need to pass this from vertex shader

uniform vec3 lightPos;
//uniform float farPlane;

void main()
{
    float farPlane = 3000.0; // or pass it as uniform
    float dist = length(position.xyz - lightPos);
    gl_FragDepth = dist / farPlane;
}
