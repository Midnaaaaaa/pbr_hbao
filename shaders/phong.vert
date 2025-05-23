#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal_matrix;

out vec3 normal_ws;
out vec3 vs_normal;
out vec3 frag_ws;

void main(void)  {
    normal_ws = normalize((transpose(inverse(model)) * vec4(normal, 0.f)).xyz);
    vs_normal = normalize(normal_matrix * normal);
    frag_ws = vec3(model * vec4(vert, 1.0));
    gl_Position = projection * view * vec4(frag_ws, 1.0);
}
