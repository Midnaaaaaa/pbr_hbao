#version 330 core

in vec2 uvs;
out vec4 frag_color;

uniform sampler2D ssaoTex;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D depth;


uniform float width;
uniform float height;
uniform float far_plane;
uniform float near_plane;
uniform float fov;

uniform bool shading;

const float PI = 3.14159265359;

float LinearizeDepth(float depth) {
    float z_ndc = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z_ndc * (far_plane - near_plane));
}

vec3 GetEyeSpacePos(vec2 uvs){
    float z_eye = LinearizeDepth(texture(depth, uvs).r);
    float x_ndc = uvs.x * 2.0 - 1.0;
    float y_ndc = uvs.y * 2.0 - 1.0;

    float halfHeight = near_plane * tan(radians(fov * 0.5));
    float halfWidth = halfHeight * (width / height);

    float x_eye = x_ndc * z_eye * (halfWidth / near_plane);
    float y_eye = y_ndc * z_eye * (halfHeight / near_plane);

    return vec3(x_eye, y_eye, -z_eye);
}

vec3 phong(vec3 light_vs, vec3 frag_vs, vec3 normal_vs){
    vec3 incidence_vec = normalize(light_vs - frag_vs);
    vec3 normalized_normal = normalize(normal_vs);

    vec3 view_dir = normalize(frag_vs);
    vec3 reflect_vec = reflect(-incidence_vec, normalized_normal);

    float ambient = 0.5;
    float diffuse = clamp(dot(normalized_normal, incidence_vec), 0, 1);
    float specular = pow(clamp(dot(view_dir, reflect_vec), 0, 1), 64);

    float light_intensity = 0.5;
    vec3 light_color = vec3(1,1,1);

    return light_intensity * light_color * (ambient + diffuse + specular);
}


void main()
{
    if(shading){
        vec3 vs_normal = texture(normal,uvs).xyz;
        vec3 albedo = texture(albedo,uvs).xyz;
        vec3 light_vs = vec3(0,1,-1);

        frag_color = vec4(albedo * texture(ssaoTex,uvs).r * phong(light_vs, GetEyeSpacePos(uvs), vs_normal),1);
    }
    else{
        vec4 texColor = texture(ssaoTex, uvs);
        frag_color = texColor;
    }
}
