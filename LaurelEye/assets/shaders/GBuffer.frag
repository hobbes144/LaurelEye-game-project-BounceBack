///////////////////////////////////////////////////////////////////////////////
// Pixel shader for GBuffer
//
// Copyright ? 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 440

#define M_PI 3.1415926535897932384626433832795

const float pi = 3.14159265358979323846;

in vec3 eyeVec;
in vec3 normalVec, worldPos;
in vec2 texCoord;
in vec3 tanVec;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

uniform vec3 diffuse;   // Kd
uniform vec3 specular;  // Ks
uniform float shininess; // alpha exponent

uniform bool useTexture;
uniform sampler2D mainTexture;
uniform vec2 mainTextureScale;

uniform bool useNormalMap;
uniform sampler2D normalMap;
uniform vec2 normalMapScale;

uniform int HDR;

uniform bool isSkydome;

layout (location = 0) out vec4 FragData[4];

vec3 toLinear(vec3 color) {
  return pow(color, vec3(2.2));
}

void main()
{
    vec2 uv;

//    uv = gl_FragCoord.xy/vec2(750,750); // (or whatever screen size)
//    FragColor = 10.0*vec3(texture(irrandianceMap, uv)); // or similar
//    return FragColor; // which disables all further code in the shader.

    vec3 V = normalize(eyeVec);

    vec3 N = normalize(normalVec);

    vec4 Kd = vec4(diffuse, 1.0f);
    vec3 Ks = specular;
    float alpha = shininess;


    // Texture mapping ...
    // This section only calculates the Kd and modified uv for some objects.
    // The modified uv ignores scaling, this is just to modify how textures
    // are handled in general, such as flipping.
    uv = texCoord;

    vec2 scaledTexCoord = uv * mainTextureScale;
    if (isSkydome) {
        scaledTexCoord = vec2(atan(V.x,V.z)/(2.0 * pi), acos(V.y)/pi);
        Kd.w = 0.0;
    }

    if (useTexture) {
        vec4 color = texture(mainTexture, scaledTexCoord);
        Kd.xyz = color.xyz;
    }

    // The normal map calc ...
    if (useNormalMap) {
      vec2 scaledTexCoord = uv * normalMapScale;
      vec3 delta = texture(normalMap, scaledTexCoord).xyz;
      delta = delta*2.0 - vec3(1.0);
      vec3 T = normalize(tanVec);
      vec3 B = normalize(cross(T,N));
      N = delta.x*T + delta.y*B + delta.z*N;
    }

    if (HDR < 1) {
      Kd.xyz = toLinear(Kd.xyz);
    }

    FragData[0] = vec4(worldPos, 1.0f);
    FragData[1] = vec4(N, 1.0f);
    FragData[2] = Kd;
    FragData[3] = vec4(Ks, alpha);


}
