#version 330

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;

in vec3 vs_normal;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal_matrix;

in vec3 ws_normal;
in vec3 frag_ws;
in vec2 uvs;

uniform sampler2D color_map;
uniform sampler2D metalness_map;
uniform sampler2D roughness_map;

uniform samplerCube diffuse_map;
uniform samplerCube prefiltered_map;
uniform sampler2D brdf_lut;

uniform int max_mips;

uniform float roughness;
uniform float metalness;

uniform int using_color_map;
uniform int using_roughness_map;
uniform int using_metalness_map;

uniform vec3 fresnel;
uniform vec3 light;

const float PI = 3.14159265359;


float D(float roughness, float NdotH)
{
    float r2 = roughness*roughness;
    float NdotH2 = NdotH * NdotH;
    return r2 / (PI * pow(NdotH2 * (r2 - 1) + 1 ,2));
}

vec3 F(vec3 F0, float LdotH){
    return F0 + (1 - F0) * pow((1 - LdotH), 5);
}

float G(float roughness, float NdotV, float NdotL, float k){
    float ggx1 = NdotV / (NdotV * (1-k) + k);
    float ggx2 = NdotL / (NdotL * (1-k) + k);

    return ggx1 * ggx2;
}


void main (void) {
    vec3 camera_pos = (inverse(view) * vec4(0,0,0,1)).xyz;
    float metalness_val = metalness;
    float roughness_val = roughness;
    vec3 albedo = vec3(1,1,1);

    if(using_color_map == 1){
        albedo = texture(color_map, uvs).rgb;
    }
    if(using_roughness_map == 1){
        roughness_val = texture(roughness_map, uvs).x;
    }
    if(using_metalness_map == 1){
        metalness_val = texture(metalness_map, uvs).x;
    }
    roughness_val = max(roughness_val, 0.001);

    vec3 N = normalize(ws_normal);
    vec3 l = normalize(light - frag_ws);
    vec3 v = normalize(camera_pos - frag_ws);
    vec3 h = normalize(l + v);

    float NdotH = max(dot(N, h), 0);
    float NdotV = max(dot(N, v), 0);
    float VdotH = max(dot(v, h), 0);
    float NdotL = max(dot(N, l), 0);
    float LdotH = max(dot(l, h), 0);


    vec3 diffuse_term = albedo / PI;

    vec3 F0 = mix(fresnel, albedo, metalness_val);
    vec3 fresnel_term = F(F0, LdotH);
    float ndf = D(roughness_val, NdotH);
    float k = pow(roughness_val + 1, 2) / 8;
    float geometric_term = G(roughness_val, NdotV, NdotL, k);

    vec3 specular_term = vec3(fresnel_term * ndf * geometric_term) / (4 * NdotL * NdotV + 0.0001);

    vec3 k_s = fresnel_term;
    vec3 k_d = vec3(1.0) - k_s;
    k_d *= (1 - metalness_val);

    vec3 BRDF = diffuse_term * k_d + specular_term;
    vec3 direct_illumination = BRDF * NdotL;

    k_s = F(F0, NdotV);
    k_d = vec3(1.0) - k_s;
    k_d *= (1.0 - metalness_val);

    vec3 R = reflect(-v, N);
    vec3 irradiance = texture(diffuse_map, N).rgb;
    vec3 ambient_specular = textureLod(prefiltered_map, R, roughness_val * (max_mips - 1.0)).rgb;
    vec2 brdf_ambient = texture2D(brdf_lut, vec2(NdotV, roughness_val)).rg;

    vec3 ambient_diffuse = irradiance * albedo;
    ambient_specular = ambient_specular * (k_s * brdf_ambient.x + brdf_ambient.y);
    vec3 ambient = k_d * ambient_diffuse + ambient_specular;

    vec3 color = direct_illumination + ambient;
    color = pow(color, vec3(1.0/2.2));

    gAlbedo = vec4(color, 1.0);
    gNormal = normalize(vs_normal);
}
