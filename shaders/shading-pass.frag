#version 330 core

in vec2 uvs;
out vec4 frag_color;

uniform sampler2D ssaoTex;
uniform sampler2D albedo;

uniform bool shading;


void main()
{
    if(shading){
        frag_color = texture(albedo,uvs) * texture(ssaoTex,uvs).r;
    }
    else{
        frag_color = texture(ssaoTex, uvs);
    }
}
