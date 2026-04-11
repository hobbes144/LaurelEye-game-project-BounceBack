#version 440
in vec2 vUV;
uniform sampler2D u_FontAtlas;
uniform vec4 u_Color;
uniform float transparency;
out vec4 FragColor;
void main()
{
    float alpha = texture(u_FontAtlas, vUV).r;
    vec4 color = vec4(u_Color.rgb, u_Color.a * alpha);
    color.a *= transparency;
    FragColor = color;
}

