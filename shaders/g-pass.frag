#version 330 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;

in vec3 vs_normal;
in vec2 uvs;

uniform sampler2D color_map;
uniform int using_color_map;

void main()
{
    gAlbedo = vec4(0.5,0.5,0.5,1);
    if(using_color_map == 1){
        gAlbedo = texture2D(color_map, uvs);
    }
    gNormal = normalize(vs_normal);
}
