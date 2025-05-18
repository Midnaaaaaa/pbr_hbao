#version 330 core

in vec2 uvs;
out vec4 frag_color;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gDepth;

uniform int current_texture;

uniform float far_plane;
uniform float near_plane;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    vec4 texColor = vec4(1,0,0,1);
    if (current_texture == 0) {
        texColor = texture(gAlbedo, uvs);
    }
    else if (current_texture == 1) {
        texColor = texture(gNormal, uvs);
        texColor = texColor * 0.5 + 0.5;
    }
    else {
        float depth = texture(gDepth, uvs).r;
        float linearDepth = LinearizeDepth(depth) / far_plane;
        texColor = vec4(vec3(linearDepth), 1.0);
    }
    frag_color = texColor;
}
