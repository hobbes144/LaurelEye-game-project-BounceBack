#version 450

in vec4 vColor;
in float vRotation;
out vec4 FragColor;

void main()
{
    // Compute rotated texture coordinates
    vec2 centered = gl_PointCoord - vec2(0.5);
    float c = cos(vRotation);
    float s = sin(vRotation);
    mat2 rot = mat2(c, -s, s, c);
    vec2 rotated = rot * centered + vec2(0.5);

    // Optional circular alpha mask (to make particles round)
    float dist = length(centered);
    if (dist > 0.5)
        discard;

    FragColor = vec4(vColor.rgb, vColor.a);
}
