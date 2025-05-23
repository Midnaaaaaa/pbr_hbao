#version 330

layout(location = 0) out vec4 gAlbedo;

uniform samplerCube specular_map;
uniform samplerCube diffuse_map;
uniform samplerCube prefiltered_map;

in vec3 tex_coords;

void main (void) {
    gAlbedo = texture(specular_map, normalize(tex_coords));
}
