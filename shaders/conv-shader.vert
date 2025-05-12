#version 330 core
layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 frag_ws;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    frag_ws = vert;
    gl_Position = projection * view * vec4(frag_ws, 1.0);
}
