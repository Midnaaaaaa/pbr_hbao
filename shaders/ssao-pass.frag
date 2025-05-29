#version 330 core

in vec2 uvs;
out vec4 frag_color;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gDepth;

uniform int current_texture;

uniform float far_plane;
uniform float near_plane;
uniform float fov;

uniform float radius;
uniform int n_samples;
uniform int n_dirs;

uniform float width;
uniform float height;


uniform mat4 projection;
uniform mat4 inv_projection;
uniform mat4 inv_view;

uniform bool ssao_improvements;

const float PI = 3.14159265359;

float random(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


float LinearizeDepth(float depth) {
    float z_ndc = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z_ndc * (far_plane - near_plane));
}

vec3 GetEyeSpacePos(vec2 uvs){
    float z_eye = LinearizeDepth(texture(gDepth, uvs).r);
    float x_ndc = uvs.x * 2.0 - 1.0;
    float y_ndc = uvs.y * 2.0 - 1.0;

    float halfHeight = near_plane * tan(radians(fov * 0.5));
    float halfWidth = halfHeight * (width / height);

    float x_eye = x_ndc * z_eye * (halfWidth / near_plane);
    float y_eye = y_ndc * z_eye * (halfHeight / near_plane);

    return vec3(x_eye, y_eye, -z_eye);
}

vec3 GetTangent(vec3 normal){
    vec3 aux;
    if(abs(normal.x) < 0.99){
        aux = vec3(1,0,0);
    }
    else{
        aux = vec3(0,1,0);
    }
    return normalize(cross(normal, aux));
}

vec3 GetBitangent(vec3 normal, vec3 tangent) {
    return normalize(cross(normal, tangent));
}

void main()
{
    vec4 texColor = vec4(1,0,0,1);
    if (current_texture == 0) {
        texColor = texture(gAlbedo, uvs);
    }
    else if (current_texture == 1) {
        texColor = texture(gNormal, uvs);
        texColor = texColor * 0.5 + 0.5;
    }
    else if (current_texture == 2){
        float depth = texture(gDepth, uvs).r;
        float linearDepth = LinearizeDepth(depth) / far_plane;
        texColor = vec4(vec3(linearDepth), 1.0);
    }
    else{
        vec3 viewPos = GetEyeSpacePos(uvs);
        vec3 viewNormal = normalize(texture(gNormal, uvs).xyz);

        float randomAngle = 0;
        float bias = 0;
        if(ssao_improvements) {
            randomAngle = random(uvs * 1000) * 2.0 * PI;
            float bias = (3.141592/360)*30.f;
        }

        vec4 radiusClip = projection * vec4(viewPos + vec3(radius,0,0), 1);
        vec2 radiusScreen = (radiusClip.xy / radiusClip.w) * 0.5 + 0.5;

        float projectedRadius = abs(radiusScreen.x - uvs.x);

        float ao = 0.0;
        for (int i = 0; i < n_dirs; i++) {
            float angle = float(i) * (2.0 * PI / float(n_dirs)) + randomAngle;
            vec2 screenDir = normalize(vec2(cos(angle), sin(angle)));
            vec3 viewDir = vec3(screenDir,0);
            vec3 tangent = viewDir - viewNormal * dot(viewNormal, viewDir);
            float tangentAngle = atan(tangent.z, length(tangent.xy));

            float maxHorizonAngle = tangentAngle;
            float horizonDist = 0;

            for (int j = 1; j <= n_samples; j++) {
                float t = projectedRadius * float(j) / float(n_samples);
                vec2 sampleUVS = uvs + screenDir * t;

                if (any(lessThan(sampleUVS, vec2(0.0))) || any(greaterThan(sampleUVS, vec2(1.0))))
                    continue;

                vec3 samplePos = GetEyeSpacePos(sampleUVS);

                vec3 v = samplePos - viewPos;
                float dist = length(v);

                if (dist < radius){
                    float horizonAngle = atan(v.z / length(v.xy));
                    if (horizonAngle > maxHorizonAngle) {
                        maxHorizonAngle = horizonAngle;
                        horizonDist = dist;
                    }
                }
            }

            float aoContribution = sin(maxHorizonAngle) - sin(tangentAngle + bias);
            ao += aoContribution;
        }
        ao = 1.0 - ao / float(n_dirs);

        texColor = vec4(vec3(ao), 1.0);
    }
    frag_color = texColor;
}
