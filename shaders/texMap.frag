#version 330

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;
in vec2 uvs;
in vec3 vs_normal;
uniform sampler2D color_map;
uniform sampler2D roughness_map;
uniform sampler2D metalness_map;

uniform int current_texture;

void main (void) {
    vec4 texColor = vec4(0,0,0,1);
    if (current_texture == 0) {
        texColor = texture(color_map, uvs);
    }
    else if (current_texture == 1) {
        texColor = texture(roughness_map, uvs);
    }
    else {
        texColor = texture(metalness_map, uvs);
    }
    gAlbedo = texColor;
    gNormal = normalize(vs_normal);
}
