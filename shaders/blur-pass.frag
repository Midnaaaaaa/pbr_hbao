#version 330 core

in vec2 uvs;
out vec4 frag_color;

uniform sampler2D ssaoTex;

const float PI = 3.14159265359;

void main()
{
    vec4 texColor = texture(ssaoTex, uvs);
    frag_color = texColor;
}
