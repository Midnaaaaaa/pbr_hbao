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

    float texelSize = 1/height;
    if(horizontal){
        texelSize = 1/width;
    }

    vec3 result = vec3(0.0);

    for (int i = 0; i < 5; i++) {
        result += texture(ssaoTex, uvs + vec2((float(i) - 2.0) * texelSize, 0.0)).rgb * weight[i];
    }

    frag_color = vec4(result, 1.0);

}
