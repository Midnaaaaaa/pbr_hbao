#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

out vec3 tex_coords;
void main(void)  {
    mat4 viewNoTranslation = view;
    viewNoTranslation[3] = vec4(0,0,0,1);
    tex_coords = vert;
    gl_Position = (projection * viewNoTranslation * vec4(vert, 1.0)).xyww;
}
