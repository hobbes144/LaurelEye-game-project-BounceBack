///////////////////////////////////////////////////////////////////////////////
// Pixel shader for UI
///////////////////////////////////////////////////////////////////////////////
#version 440

#define M_PI 3.1415926535897932384626433832795

in vec2 texCoord;

uniform float transparency;

uniform bool useTexture;
uniform sampler2D mainTexture;  
uniform vec2 mainTextureScale;
uniform vec4 u_Color;

out vec4 FragColor;

void main()
{
    vec4 color = u_Color;

    if(useTexture){
        vec2 scaledTexCoord = texCoord * mainTextureScale;
        vec4 texColor = texture(mainTexture, scaledTexCoord);
        color *= texColor;
    }

    color.a *= transparency;

    FragColor = color;
}
