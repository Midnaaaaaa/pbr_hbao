#version 330 core
layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 ws_frag;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    ws_frag = vert;
    gl_Position =  projection * view * vec4(ws_frag, 1.0);
}
