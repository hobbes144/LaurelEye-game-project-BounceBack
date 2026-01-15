///////////////////////////////////////////////////////////////////////////////
// Pixel shader for lighting
//
// Copyright ? 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 440

#define M_PI 3.1415926535897932384626433832795

const uint UINT_MAX = 0xFFFFFFFFU; // or 4294967295U;
const float pi = 3.14159265358979323846;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

struct DirectionalLight {
  vec3 direction;
  float intensity;
  vec3 color;
  uint shadowIndex;
};

struct AmbientLight {
  vec3 color;
  float intensity;
};

layout (binding = 1) uniform lights
{
  DirectionalLight sunLight;
  AmbientLight ambientLight;
};

struct ShadowResource {
    mat4 shadowMatrix;
    uint framebuffer;
    uint shadowMap;
    uint lightIndex;
    uint type;
};

layout(std430, binding = 10) buffer shadows {
    ShadowResource shadowResources[];
};

uniform sampler2D ShadowMap;

uniform bool HDR;

uniform uint height, width;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_diffuse;
uniform sampler2D gbuffer_specular;

out vec3 FragColor;

vec3 toSRGB(vec3 color) {
  const float exposure = 1.0f;
  return pow(
      (
        (exposure * color) /
        (
          (exposure * color) +
          vec3(1.0)
        )
      ), vec3(1.0/2.2));
}

// L = Light vector
// N = Normal vector
// V = Eye vector
// Ii = Light color at Light vector
// Kd = Diffuse color of surface
// Ks = Specular color of surface
// alpha = Shininess
vec3 specularBRDF(
  const vec3 L, const vec3 N, const vec3 V,
  const vec3 Ii,
  const vec3 Kd, const vec3 Ks, const float alpha) {

  const vec3 H = normalize(L+V);
  const float LN = max(dot(L,N),0.0);
  const float HN = max(dot(H,N),0.0);
  const float VN = max(dot(V,N),0.0);
  const float LH = max(dot(L,H),0.0);

  // BRDF
  const vec3 F = Ks + ((vec3(1.0,1.0,1.0)-Ks)*pow((1.0-LH),5.0));
  const float D = ((alpha+2.0)/(2.0*M_PI))*(pow(HN,alpha));
  const vec3 BRDF = (Kd/M_PI) + ((F*D)/(4*pow(max(LH,0.0000000001),2.0)));
  return Ii*LN*BRDF;
}

void main()
{
    vec2 gBufferPosition = gl_FragCoord.xy/vec2(width,height);

    vec3 worldPos = texture(gbuffer_position, gBufferPosition).xyz;

    bool inShadow = false;
    if (sunLight.shadowIndex < UINT_MAX - 1) {
      const vec4 shadowCoord = shadowResources[sunLight.shadowIndex].shadowMatrix * vec4(worldPos, 1.0);
      vec2 shadowIndex = shadowCoord.xy/shadowCoord.w;
      if (shadowCoord.w > 0 &&
        shadowIndex.x >= 0.0f && shadowIndex.x < 1.0f &&
        shadowIndex.y >= 0.0f && shadowIndex.y < 1.0f) {
        float lightDepth = texture(ShadowMap, shadowIndex).x;
        float pixelDepth = shadowCoord.z/shadowCoord.w;
        if ( pixelDepth > (lightDepth + 0.0001f) )
          inShadow = true;
      }
    }

    vec3 eyeVec = (inverseView*vec4(0.0,0.0,0.0,1.0)).xyz-worldPos;

    vec3 N = texture(gbuffer_normal, gBufferPosition).xyz;
    vec3 V = normalize(eyeVec);

    vec3 Kd = texture(gbuffer_diffuse, gBufferPosition).xyz;
    vec3 Ks = texture(gbuffer_specular, gBufferPosition).xyz;
    float alpha = texture(gbuffer_specular, gBufferPosition).w;
    // Retaining transparency value.

    vec3 Ia = ambientLight.color * ambientLight.intensity;

    vec3 Ii = sunLight.color * sunLight.intensity;
    vec3 L = -normalize(sunLight.direction);

    FragColor = vec3(0.0,0.0,0.0);

    if (!inShadow) {
        FragColor = specularBRDF(L, N, V, Ii, Kd, Ks, alpha);
    }

    if (texture(gbuffer_diffuse, gBufferPosition).w == 0.0f) {
      FragColor = toSRGB(Kd);
      return;
    }


    vec3 ambientDiffuse = Ia*Kd;
    FragColor += ambientDiffuse;

    FragColor = toSRGB(FragColor);
}
