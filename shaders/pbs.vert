#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal_matrix;

out vec3 ws_normal;
out vec3 frag_ws;
out vec2 uvs;

void main(void){
    uvs = texCoord;
    ws_normal = normalize((transpose(inverse(model)) * vec4(normal, 0.f)).xyz);
    frag_ws = (model * vec4(vert, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(vert, 1.0);
}
