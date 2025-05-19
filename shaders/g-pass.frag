#version 330 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;

in vec3 vs_normal;
in vec2 uvs;
in vec3 vs_frag;

uniform sampler2D color_map;
uniform int using_color_map;

void main()
{
    gAlbedo = vec4(0.5,0.5,0.5,1);
    if(using_color_map == 1){
        gAlbedo = texture2D(color_map, uvs);
    }
    vec3 d_x = dFdx(vs_frag);
    vec3 d_y = dFdy(vs_frag);
        
    gNormal = normalize(cross(d_x, d_y));;
    
    //gNormal = normalize(vs_normal);
}
