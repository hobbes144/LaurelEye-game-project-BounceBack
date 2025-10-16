/////////////////////////////////////////////////////////////////////////
// Vertex shader for lighting
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 440

in vec4 vertex;

uniform uint height, width;

out vec3 FragColor;


void main() {
    FragColor = vec3(gl_FragCoord.xy/vec2(width,height), 1.0f);
}
