///////////////////////////////////////////////////////////////////////////////
// Pixel shader for rendering Point Lights
//
// Copyright ? 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 440

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

uniform samplerCube shadowCubeMaps[MAX_SHADOWS];

uniform uint lightIndex;

uniform uint height, width;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_diffuse;
uniform sampler2D gbuffer_specular;

out vec3 FragColor;

float computePointShadow(uint shadowIdx, vec3 fragPos, vec3 lightPos, vec3 N)
{
    float normalBiasAmount = 0.2;
    vec3 biasedPos = fragPos + N * normalBiasAmount;

    vec3 L = biasedPos - lightPos;
    float currentDepth = length(L);
    vec3 dir = normalize(L);

    vec3 lightDir = normalize(lightPos - fragPos);
    float bias = max(lights[lightIndex].radius * 0.0005 * (1.0 - dot(N, lightDir)), 
                     lights[lightIndex].radius * 0.0005);

    vec3 sampleOffsetDirections[20] = vec3[](
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    float diskRadius = (1.0 + (currentDepth / lights[lightIndex].radius)) / 500.0;

    float shadow = 0.0;
    int samples = 20;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowCubeMaps[shadowIdx], dir + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= lights[lightIndex].radius;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow / float(samples);
}

// Quantize a 0..1 value into N hard bands
float celQuantize(float value, float bands)
{
    return floor(value * bands) / bands;
}

void main()
{
    vec2 gBufferPosition = gl_FragCoord.xy / vec2(width, height);

    vec3 worldPos = texture(gbuffer_position, gBufferPosition).xyz;

    const Light light = lights[lightIndex];

    vec3 toLight = light.position - worldPos;
    float radius = light.radius;
    float lightDistance = length(toLight);

    vec3 N = texture(gbuffer_normal, gBufferPosition).xyz;
    vec3 L = normalize(toLight);
    vec3 V = normalize((inverseView * vec4(0,0,0,1)).xyz - worldPos);
    vec3 H = normalize(L + V);

    vec3 Kd = texture(gbuffer_diffuse, gBufferPosition).xyz;
    vec3 Ks = texture(gbuffer_specular, gBufferPosition).xyz;
    float alpha = texture(gbuffer_specular, gBufferPosition).w;

    vec3 Ii = light.color * light.intensity;

    float attenuation = max(0.0, 1.0 / pow(lightDistance, 2) - 1.0 / pow(radius, 2));

//    // -------------------------------------------------------------------------
//    // HARD CEL SHADING (active)
//    // -------------------------------------------------------------------------
    float NdotL = max(dot(N, L), 0.0);
    // Add a small epsilon guard so band 0 is truly dark
    float celDiffuse = NdotL < 0.01 ? 0.0 : celQuantize(NdotL, 3.0);

    // Hard specular — on or off based on threshold
    float NdotH = max(dot(N, H), 0.0);
    // Remap pow(NdotH, alpha) so the threshold is relative to alpha
    float specPower = pow(NdotH, alpha);
    // Use a tighter threshold that scales with shininess
    float specThreshold = pow(0.5, 1.0 / max(alpha, 1.0));
    float celSpecular = (alpha > 1.0 && pow(NdotH, alpha) > 0.95) ? 1.0 : 0.0;

    vec3 diffuseColor  = Kd * celDiffuse;
    vec3 specularColor = Ks * 0;

    FragColor = Ii * (diffuseColor + specularColor) * attenuation;

    // -------------------------------------------------------------------------
    // PBR BRDF VERSION (commented out)
    // -------------------------------------------------------------------------
    // float LN = max(dot(L, N), 0.0);
    // float HN = max(dot(H, N), 0.0);
    // float VN = max(dot(V, N), 0.0);
    // float LH = max(dot(L, H), 0.0);
    //
    // vec3 F = Ks + ((vec3(1,1,1) - Ks) * pow((1 - LH), 5.0));
    // float D = ((alpha + 2.0) / (2 * pi)) * pow(HN, alpha);
    // vec3 BRDF = (Kd / pi) + ((F * D) / (4 * pow(max(LH, 0.0000000001), 2.0)));
    // FragColor = Ii * LN * BRDF * attenuation;
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // SOFT GRADIENT BANDS VERSION (commented out)
    // -------------------------------------------------------------------------
    // float NdotL = max(dot(N, L), 0.0);
    // float smoothDiffuse = smoothstep(0.0, 0.1, NdotL); // soft edge at terminator
    // float celDiffuse    = celQuantize(smoothDiffuse, 3.0);
    // float NdotH         = max(dot(N, H), 0.0);
    // float celSpecular   = smoothstep(0.48, 0.52, pow(NdotH, alpha)); // soft specular edge
    // FragColor = Ii * (Kd * celDiffuse + Ks * celSpecular) * attenuation;
    // -------------------------------------------------------------------------

    // Shadow
    float shadow = 0.0;
    if (light.shadowIndex != 0xFFFFFFFFu) {
        shadow = computePointShadow(light.shadowIndex, worldPos, light.position, N);
    }

    FragColor *= (1.0 - shadow);

    if (!light.isActive || lightDistance > radius || texture(gbuffer_diffuse, gBufferPosition).w == 0.0)
        discard;
}
