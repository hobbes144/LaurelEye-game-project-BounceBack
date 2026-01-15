///////////////////////////////////////////////////////////////////////////////
// Pixel shader for rendering Point Lights
//
// Copyright ? 2025 DIGIPEN Institute of Technology. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////
#version 440

const float pi = 3.14159265358979323846;

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

uniform uint height, width;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_diffuse;
uniform sampler2D gbuffer_specular;

out vec3 FragColor;

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

    float attenuation =
      (1/pow(lightDistance, 2) - 1/pow(radius, 2));


    FragColor *= attenuation;

    if ( lights[lightIndex].isActive == false || lightDistance > radius  || texture(gbuffer_diffuse, gBufferPosition).w == 0.0f )
      discard;
}
