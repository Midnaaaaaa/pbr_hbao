#version 330

out vec4 frag_color;
in vec2 uvs;
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
    frag_color = texColor;
}
