#version 330 core

layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 texCoords;

out vec2 uvs;

void main()
{
    uvs = texCoords;
    gl_Position = vec4(vert.xy, 0.0, 1.0);
}
