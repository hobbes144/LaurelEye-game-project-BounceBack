///////////////////////////////////////////////////////////////////////////////
// Vertex shader for rendering Point Lights
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 440

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;
layout(location = 4) in vec3 vertexTangent;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

struct Light {
  vec3 position;
  float intensity;
  vec3 color;
  float radius;

  uint shadowIndex;

  bool isActive;

  int padding[2];
};

layout(std430, binding = 2) buffer LightBuffer {
    Light lights[];
};

uniform uint lightIndex;

void main()
{
    // Computes world position at a pixel used for
    // light and eye vector calculations
    const vec4 worldPos=vec4((vertex.xyz * lights[lightIndex].radius) + lights[lightIndex].position, 1.0f);

    // Computes the points projection on the screen
    gl_Position = projection * view * worldPos;
    //gl_Position = projection*view*vec4( ((vertex.xyz * lights[lightIndex].radius) + lights[lightIndex].position), 1.0 );
}
