/////////////////////////////////////////////////////////////////////////
// Pixel shader for lighting
////////////////////////////////////////////////////////////////////////
#version 430 core

#define M_PI 3.1415926535897932384626433832795

const float pi = 3.14159265358979323846;

//out vec4 FragColor;

// These definitions agree with the ObjectIds enum in scene.h
const int     nullId	= 0;
const int     skyId	= 1;
const int     seaId	= 2;
const int     groundId	= 3;
const int     roomId	= 4;
const int     boxId	= 5;
const int     frameId	= 6;
const int     lPicId	= 7;
const int     rPicId	= 8;
const int     teapotId	= 9;
const int     spheresId	= 10;
const int     floorId	= 11;

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

struct DirectionalLight {
  vec3 direction;
  float intensity;
  vec3 color;
  float padding0;
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

uniform vec3 diffuse;   // Kd
uniform vec3 specular;  // Ks
uniform float shininess; // alpha exponent

uniform bool useTexture;
uniform sampler2D mainTexture;
uniform vec2 mainTextureScale;

uniform bool useNormalMap;
uniform sampler2D normalMap;
uniform vec2 normalMapScale;

uniform int skydome;
uniform int HDR;

uniform int objectId;
uniform int mode;

out vec4 FragColor;

vec3 toLinear(vec3 color) {
  return pow(color, vec3(2.2));
}

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

vec2 uvFromDirectionVector(vec3 D) {
  return vec2(atan(-D.z,D.x)/(2.0f*pi), acos(D.y)/pi);
}

vec2 uvFromDirectionVectorFlipped(vec3 D) {
  return vec2(0.5 - atan(-D.z,D.x)/(2.0f*pi), acos(D.y)/pi);
}

vec3 directionVectorFromSkewAngle(vec2 uv) {

  //vec3 result = vec3()

  return vec3(
    cos(2.0 * pi * ( 0.5 - uv.x )) * sin(pi * uv.y),
    cos(pi * uv.y),
    -sin(2.0 * pi * ( 0.5 - uv.x )) * sin(pi * uv.y)
    );
}

vec3 directionVectorFromUVFlipped(vec2 uv) {
  return vec3(
    cos(2 * pi * ( 0.5 - uv.y )) * sin(pi * uv.x),
    sin(2 * pi * ( 0.5 - uv.y )) * sin(pi * uv.x),
    cos(pi * uv.y)
    );
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

    vec2 uv;

    vec3 eyeVec = (inverseView*vec4(0.0,0.0,0.0,1.0)).xyz-worldPos;

    vec3 N = normalize(normalVec);
    vec3 V = normalize(eyeVec);

    vec3 Kd = diffuse;
    vec3 Ks = specular;
    float alpha = shininess;

    vec3 Ia = ambientLight.color * ambientLight.intensity;

    vec3 Ii = sunLight.color * sunLight.intensity;
    vec3 L = -normalize(sunLight.direction);

    uv = texCoord;

    vec2 scaledTexCoord = uv * mainTextureScale;

    if (useTexture) {
      vec4 color = texture(mainTexture, scaledTexCoord);
      Kd = color.xyz;
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

    if (skydome > 0) {
      vec3 eyeVec = (inverseView*vec4(0.0,0.0,0.0,1.0)).xyz-worldPos;
      vec3 V = normalize(eyeVec);
      vec2 uv = vec2(0.5 -atan(-V.z,V.x)/(2.0f*pi), acos(V.y)/pi);
      Kd = texture(mainTexture, uv).xyz;
      FragColor = vec4(toSRGB(Kd), 1.0);
      return;

    }

    if (HDR < 1) {
      Kd = toLinear(Kd);
    }

    if (skydome > 0) {
      FragColor = vec4(toSRGB(Kd), 1.0);
      return;
    }

    vec3 ambientDiffuse;

    // A checkerboard pattern to break up large flat expanses.  Remove when using textures.
    if (objectId==groundId || objectId==floorId || objectId==seaId) {
        ivec2 uv = ivec2(floor(100.0*texCoord));
        if ((uv[0]+uv[1])%2==0)
            Kd *= 0.9; }


    FragColor = vec4(specularBRDF(L, N, V, Ii, Kd, Ks, alpha), 1.0);
    ambientDiffuse = Ia*Kd;

    FragColor += vec4(ambientDiffuse, 1.0);

    FragColor.xyz = toSRGB(FragColor.xyz);
}
