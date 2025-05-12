#version 330 core
out vec2 frag_color;
in vec2 brdf_lut_coords;

const float PI = 3.14159265359;
const int NUM_SAMPLES = 1000;

//https://learnopengl.com/PBR/IBL/Diffuse-irradiance
float RadicalInverse_VdC(uint bits){
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
//https://learnopengl.com/PBR/IBL/Diffuse-irradiance
vec2 Hammersley(int i, int N){
    return vec2(float(i)/float(N), RadicalInverse_VdC(uint(i)));
}

vec3 GenerateGGXSample(vec2 randomSample, vec3 N, float roughness){
    float r2 = roughness*roughness;

    float phi = 2.0 * PI * randomSample.x;
    float cosTheta2 = (1.0 - randomSample.y) / (1.0 + (r2 * r2 - 1.0) * randomSample.y);
    float cosTheta = sqrt(cosTheta2);
    float sinTheta = sqrt(1.0 - cosTheta2);

    vec3 localH;
    localH.x = cos(phi) * sinTheta;
    localH.y = sin(phi) * sinTheta;
    localH.z = cosTheta;

    vec3 basis_vec;
    if (abs(N.x) > 0.99) {
        basis_vec = vec3(0.0, 1.0, 0.0);
    } else {
        basis_vec = vec3(1.0, 0.0, 0.0);
    }

    vec3 tangent = normalize(cross(N, basis_vec));
    vec3 binormal = cross(N, tangent);
    vec3 worldH = tangent * localH.x + binormal * localH.y + N * localH.z;

    return normalize(worldH);
}

float G(float roughness, float NdotV, float NdotL, float k){
    float ggx1 = NdotV / (NdotV * (1-k) + k);
    float ggx2 = NdotL / (NdotL * (1-k) + k);

    return ggx1 * ggx2;
}


void main() {
    float NdotV = brdf_lut_coords.x;
    float roughness = brdf_lut_coords.y;

    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float scale = 0.0;
    float bias = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);
    float k = roughness * roughness / 2;


    for(int i = 0; i < NUM_SAMPLES; ++i)
    {
        vec2 randomSample = Hammersley(i, NUM_SAMPLES);
        vec3 H = GenerateGGXSample(randomSample, N, roughness);
        vec3 L = reflect(-V,H);

        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float geometric_term = G(roughness, NdotV, NdotL, k);
            float geometric_visibility_factor = (geometric_term * VdotH) / (NdotH * NdotV);
            float F = pow(1.0 - VdotH, 5.0);

            scale += (1.0 - F) * geometric_visibility_factor;
            bias += F * geometric_visibility_factor;
        }
    }
    scale /= float(NUM_SAMPLES);
    bias /= float(NUM_SAMPLES);
    frag_color = vec2(scale, bias);
}
