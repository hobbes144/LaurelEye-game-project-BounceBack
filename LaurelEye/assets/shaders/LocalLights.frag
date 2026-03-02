///////////////////////////////////////////////////////////////////////////////
// Pixel shader for rendering Point Lights
//
// Copyright ? 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 450

const float pi = 3.14159265358979323846;
const int MAX_SHADOWS = 16;

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


uniform uint height, width;
uniform uint lightIndex;

layout(binding = 0) uniform sampler2D gbuffer_position;
layout(binding = 1) uniform sampler2D gbuffer_normal;
layout(binding = 2) uniform sampler2D gbuffer_diffuse;
layout(binding = 3) uniform sampler2D gbuffer_specular;
layout(binding = 10) uniform samplerCube shadowCubeMaps[MAX_SHADOWS];

out vec3 FragColor;

// from learn opengl
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
float computePointShadow(uint shadowIdx, vec3 fragPos, vec3 lightPos, vec3 N)
{
    float normalBiasAmount = 0.2;
    vec3 biasedPos = fragPos + N * normalBiasAmount;

    // get vector between fragment position and light position
    vec3 L = biasedPos - lightPos;

    // get current linear depth as the length between the fragment and light position
    float currentDepth = length(L);

    vec3 dir = normalize(L);

    // use the light to fragment vector to sample from the depth map
    // Sample depth from cube map
    float closestDepth = texture(shadowCubeMaps[shadowIdx], dir).r;

    // it is currently in linear range between [0,1]. Re-transform back to original value
    // Scale back to world units
    closestDepth *= lights[lightIndex].radius; // or farPlane if you store it

    // now test for shadows
    vec3 lightDir = normalize(lightPos - fragPos);
    float bias = max(lights[lightIndex].radius * 0.005 * (1.0 - dot(N, lightDir)),
                 lights[lightIndex].radius * 0.0005);
    //float offset = 2.1; // tune this - push in world units
    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}

void main()
{
    vec2 gBufferPosition = gl_FragCoord.xy/vec2(width,height);

    vec3 worldPos = texture(gbuffer_position, gBufferPosition).xyz;

    const Light light = lights[lightIndex];

    vec3 L = light.position - worldPos;
    float radius = light.radius;
    float lightDistance = length(L);

    float G = 0.0;

    vec3 eyeVec = (inverseView*vec4(0,0,0,1)).xyz-worldPos;

    vec3 N = texture(gbuffer_normal, gBufferPosition).xyz;
    vec3 V = normalize(eyeVec);

    vec3 Kd = texture(gbuffer_diffuse, gBufferPosition).xyz;
    vec3 Ks = texture(gbuffer_specular, gBufferPosition).xyz;
    float alpha = texture(gbuffer_specular, gBufferPosition).w;

    vec3 Ii = lights[lightIndex].color * lights[lightIndex].intensity;

    L = normalize(L);

    // The lighting calculation ...
    vec3 H = normalize(L+V);
    float LN = max(dot(L,N),0.0);
    float HN = max(dot(H,N),0.0);
    float VN = max(dot(V,N),0.0);
    float LH = max(dot(L,H),0.0);

    // BRDF
    vec3 F = Ks + ((vec3(1,1,1)-Ks)*pow((1-LH),5.0));
    float D = ((alpha+2.0)/(2*pi))*(pow(HN,alpha));
    vec3 BRDF = (Kd/pi) + ((F*D)/(4*pow(max(LH,0.0000000001),2.0)));
    FragColor = (Ii*LN*BRDF);

    float constant  = 1.0;
    float linear    = 0.7;
    float quadratic = 1.8;


//    float attenuation =
//      1.0 / (constant + linear * lightDistance + quadratic * (lightDistance * lightDistance));
//
//    float attenuation =
//      (1/pow(lightDistance, 2) - 1/pow(radius, 2));

    float attenuation = max(0.0, 1.0/pow(lightDistance,2) - 1.0/pow(radius,2));
    FragColor *= attenuation;


    // Shadow
    float shadow = 0.0;
    if (light.shadowIndex != 0xFFFFFFFFu) {
        shadow = computePointShadow(light.shadowIndex, worldPos, light.position,N);
    }

    FragColor *= (1.0 - shadow);

    if ( lights[lightIndex].isActive == false || lightDistance > radius  || texture(gbuffer_diffuse, gBufferPosition).w == 0.0f )
      discard;
}
