///////////////////////////////////////////////////////////////////////////////
// Fragment shader for Shadow
//
// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 450

in vec4 position;
out vec4 FragColor;

void main()
{
    float z_ndc = position.z / position.w;
    float z01   = z_ndc * 0.5 + 0.5;
  FragColor = vec4(
    z01,
    pow(z01, 2.0),
    pow(z01, 3.0),
    pow(z01, 4.0));
}
