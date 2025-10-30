///////////////////////////////////////////////////////////////////////////////
// Pixel shader for UI
///////////////////////////////////////////////////////////////////////////////
#version 440

#define M_PI 3.1415926535897932384626433832795

const float pi = 3.14159265358979323846;

in vec2 texCoord;

layout (binding = 0) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    float exposure;
};

uniform float transparency;

uniform bool useTexture;
uniform sampler2D mainTexture;
uniform vec2 mainTextureScale;

out vec3 FragColor;

void main()
{
    vec2 scaledTexCoord = texCoord * mainTextureScale;
    float x = scaledTexCoord.x;
    float y = scaledTexCoord.y;
    scaledTexCoord = vec2(x, 1-y);
    FragColor = texture(mainTexture, scaledTexCoord).xyz * transparency;
}
