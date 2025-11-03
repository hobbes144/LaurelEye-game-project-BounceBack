///////////////////////////////////////////////////////////////////////////////
// Pixel shader for Skydome
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 430

const float pi = 3.14159265358979323846;

in vec3 eyePos;
uniform sampler2D skydomeTexture;

uniform bool HDR;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

out vec4 FragData;

vec3 toSRGB(vec3 color) {
  return pow(
      (
        (exposure * color) /
        (
          (exposure * color) +
          vec3(1.0)
        )
      ), vec3(1/2.2));
}

void main()
{
    vec3 V = normalize(eyePos);

    // Sky skips light processing so using a shortcut.
    vec2 uv = vec2(atan(V.x,V.z)/(2.0 * pi), acos(V.y)/pi);
    vec4 color = texture(skydomeTexture, uv);
    if (HDR) {
      color.xyz = toSRGB(color.xyz);
    }
    FragData = color;
}
