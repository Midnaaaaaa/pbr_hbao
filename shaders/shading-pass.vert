#version 330

layout (location = 0) in vec2 vert;
layout (location = 1) in vec2 texCoord;

out vec2 uvs;

void main()
{
    uvs = texCoord;
    gl_Position = vec4(vert, 0, 1.0);
}
