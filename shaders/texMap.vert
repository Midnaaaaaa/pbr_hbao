#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 uvs;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main(void)  {
    uvs = texCoord;
    gl_Position = projection * view * model * vec4(vert, 1.0);
}
