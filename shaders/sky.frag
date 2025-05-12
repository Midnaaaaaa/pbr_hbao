#version 330

out vec4 frag_color;
uniform samplerCube specular_map;
uniform samplerCube diffuse_map;
uniform samplerCube prefiltered_map;

in vec3 tex_coords;

void main (void) {
    frag_color = texture(specular_map, normalize(tex_coords));
}
