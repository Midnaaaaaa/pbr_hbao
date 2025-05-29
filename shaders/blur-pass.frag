#version 330 core

out vec4 frag_color;
in vec2 uvs;

uniform sampler2D ssaoTex;
uniform float width;
uniform float height;
uniform bool horizontal;

void main()
{
    float weight[5] = float[] (0.06136, 0.24477, 0.38774, 0.24477, 0.06136);

    float texelSize = horizontal ? 1.0 / width : 1.0 / height;

    vec3 result = vec3(0.0);
    for (int i = 0; i < 5; i++) {
        float offset = (float(i) - 2.0) * texelSize;
        vec2 delta = horizontal ? vec2(offset, 0.0) : vec2(0.0, offset);
        result += texture(ssaoTex, uvs + delta).rgb * weight[i];
    }

    frag_color = vec4(result, 1.0);
}
