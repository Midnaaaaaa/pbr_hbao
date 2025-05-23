#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 ws_normal;
out vec3 vs_normal;
out vec3 frag_ws;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal_matrix;


void main(void)  {
    ws_normal = normalize((transpose(inverse(model)) * vec4(normal, 0.f)).xyz);
    vs_normal = normalize(normal_matrix * normal);
    frag_ws = (model * vec4(vert, 1.0)).xyz;
    gl_Position = projection * view * vec4(frag_ws, 1.0f);
}
