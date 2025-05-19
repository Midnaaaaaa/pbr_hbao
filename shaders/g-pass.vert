#version 330
layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 vs_normal;
out vec2 uvs;
out vec3 vs_frag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main()
{
    vs_frag = (view * model * vec4(vert, 1.0)).xyz;
    vs_normal = normalize(normal_matrix * normal);
    uvs = texCoord;
    gl_Position = projection * view * model * vec4(vert, 1.0);
}
