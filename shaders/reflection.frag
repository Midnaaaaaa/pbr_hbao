#version 330

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;
in vec3 frag_ws;
in vec3 ws_normal;
in vec3 vs_normal;  // View-space normal for G-buffer

uniform samplerCube specular_map;
uniform mat4 view;

void main (void) {
    vec3 camera_ws = (inverse(view) * vec4(0,0,0,1)).xyz;
    vec3 incidence_vec = normalize(camera_ws - frag_ws);
    vec3 reflected_vec = normalize(reflect(-incidence_vec, ws_normal));
    gAlbedo = texture(specular_map, reflected_vec);
    // Store view-space normal in G-buffer
    gNormal = normalize(vs_normal);
}
