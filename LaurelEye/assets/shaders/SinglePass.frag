/////////////////////////////////////////////////////////////////////////
// Pixel shader for lighting
//
// For LaurelEyeEngine
////////////////////////////////////////////////////////////////////////
#version 430 core

#define M_PI 3.1415926535897932384626433832795

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

in vec3 normalVec, lightVec, eyeVec;
in vec2 texCoord;

uniform int objectId;
uniform int mode;
uniform vec3 diffuse;   // Kd
uniform vec3 specular;  // Ks
uniform float shininess; // alpha exponent

uniform vec3 Light;     // Ii
uniform vec3 Ambient;   // Ia

out vec3 FragColor;

void main()
{
    vec3 N = normalize(normalVec);
    vec3 L = normalize(lightVec);
    vec3 V = normalize(eyeVec);

    vec3 Kd = diffuse;
    vec3 Ii = Light;
    vec3 Ia = Ambient;
    vec3 Ks = specular;
    float alpha = shininess;

    // A checkerboard pattern to break up large flat expanses.  Remove when using textures.
    if (objectId==groundId || objectId==floorId || objectId==seaId) {
        ivec2 uv = ivec2(floor(100.0*texCoord));
        if ((uv[0]+uv[1])%2==0)
            Kd *= 0.9; }

    // The lighting calculation ...
    vec3 H = normalize(L+V);
    float LN = max(dot(L,N),0.0);
    float HN = max(dot(H,N),0.0);
    float VN = max(dot(V,N),0.0);
    float LH = max(dot(L,H),0.0);

    if (mode == 0) {
      FragColor = vec3(0.5,0.5,0.5)*Kd + Kd*max(dot(L,N),0.0);
    }

    if (mode == 1) {
      // Phong
      FragColor = (Ia*Kd) + (Ii*Kd*LN) + (Ii*Ks*pow(HN,alpha));
    }

    if (mode == 2) {
      // BRDF
      vec3 F = Ks + (((1,1,1)-Ks)*pow((1-LH),5.0));
      float D = ((alpha+2.0)/(2*M_PI))*(pow(HN,alpha));
      vec3 BRDF = (Kd/M_PI) + ((F*D)/(4*pow(max(LH,0.0000000001),2.0)));
      FragColor = (Ia*Kd) + (Ii*LN*BRDF);
    }

}
