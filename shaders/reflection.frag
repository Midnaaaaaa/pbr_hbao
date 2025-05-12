#version 330

out vec4 frag_color;
in vec3 frag_ws;
in vec3 ws_normal;

uniform samplerCube specular_map;
uniform mat4 view;

void main (void) {
    vec3 camera_ws = (inverse(view) * vec4(0,0,0,1)).xyz;
    vec3 incidence_vec = normalize(camera_ws - frag_ws);
    vec3 reflected_vec = normalize(reflect(-incidence_vec, ws_normal));
    frag_color = texture(specular_map, reflected_vec);
}
