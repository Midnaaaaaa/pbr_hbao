#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 uvs;
out vec3 vs_normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main(void)  {
    uvs = texCoord;
    vs_normal = normalize(mat3(transpose(inverse(view * model))) * normal);
    gl_Position = projection * view * model * vec4(vert, 1.0);
}
