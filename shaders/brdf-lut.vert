#version 330 core
layout (location = 0) in vec2 vert;
layout (location = 1) in vec2 uvs;


out vec2 brdf_lut_coords;

void main()
{
    brdf_lut_coords = uvs;
    gl_Position = vec4(vert, 0, 1);
}
