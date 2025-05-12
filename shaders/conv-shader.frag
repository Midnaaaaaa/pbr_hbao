#version 330 core
out vec4 frag_color;
in vec3 frag_ws;

uniform samplerCube environmentMap;
const float PI = 3.14159265359;

void main() {
    vec3 N = normalize(frag_ws);
    vec3 irradiance = vec3(0.0);

    vec3 tangent = vec3(0.0, 1.0, 0.0);
    vec3 binormal = normalize(cross(tangent, N));
    tangent = normalize(cross(N, binormal));


    float num_sample = 512;
    float sampleDelta = (PI * PI) / num_sample;
    float num_samples = 0.0;

    //https://learnopengl.com/PBR/IBL/Diffuse-irradiance
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            vec3 sample_vec_local_space = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sample_vec = sample_vec_local_space.x * binormal + sample_vec_local_space.y * tangent + sample_vec_local_space.z * N;
            irradiance += texture(environmentMap, sample_vec).rgb * cos(theta) * sin(theta);
            num_samples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(num_samples));
    frag_color = vec4(irradiance, 1.0);
}
