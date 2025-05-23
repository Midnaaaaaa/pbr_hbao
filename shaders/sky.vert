#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

out vec3 tex_coords;

void main(void) {
    // Remove translation from the view matrix to keep the skybox centered on the camera
    mat4 viewNoTranslation = view;
    viewNoTranslation[3] = vec4(0.0, 0.0, 0.0, 1.0);
    
    // Set texture coordinates for cubemap lookup
    tex_coords = vert;
    
    // Transform position to clip space and set z = w to ensure depth is 1.0 (far plane)
    // The .xyww swizzle ensures z = w, which normalizes to 1.0 in NDC
    vec4 pos = projection * viewNoTranslation * vec4(vert, 1.0);
    gl_Position = pos.xyww;
}
