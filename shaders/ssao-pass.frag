#version 330 core

in vec2 uvs;
out vec4 frag_color;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gDepth;

uniform int current_texture;

uniform float far_plane;
uniform float near_plane;

uniform float radius;
uniform int n_samples;
uniform int n_dirs;

uniform float width;
uniform float height;
uniform float fov;

uniform mat4 projection;
uniform mat4 inv_projection;
uniform mat4 inv_view;

const float PI = 3.14159265359;
const float EPSILON = 0.0001;

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
        vec3 viewNormal = texture(gNormal, uvs).xyz;

        // Initialize AO
        float ao = 0.0;

        // Convert radius to pixel space
        vec2 pixelSize = 1.0 / vec2(width, height);
        float pixelRadius = radius * 0.5 * height / (viewPos.z * tan(radians(fov * 0.5)));

        // Get random rotation angle for this pixel
        float randomAngle = random(uvs * 1000.0) * 2.0 * PI;

        // For each direction
        for (int i = 0; i < n_dirs; i++) {
            // Calculate direction in view space
            float angle = float(i) * (2.0 * PI / float(n_dirs)) + randomAngle;
            vec2 dir = vec2(cos(angle), sin(angle));

            // Initialize horizon vector and maximum angle
            vec3 horizonVec = vec3(0.0);
            float maxHorizonAngle = -PI * 0.5; // Start with minimum possible angle

            // For each sample along the direction
            for (int j = 1; j <= n_samples; j++) {
                // Calculate sample position in pixel space
                float t = float(j) / float(n_samples);
                vec2 offset = dir * t * pixelRadius;

                // Convert back to UV space
                vec2 sampleUV = uvs + offset * pixelSize;

                // Skip if out of screen
                if (any(lessThan(sampleUV, vec2(0.0))) || any(greaterThan(sampleUV, vec2(1.0))))
                    continue;

                // Get sample position in view space
                vec3 samplePos = GetEyeSpacePos(sampleUV);

                // Calculate vector from current point to sample point
                vec3 v = samplePos - viewPos;
                float dist = length(v);

                // Skip if sample is behind the surface or too far
                if (dist > radius || dot(v, viewNormal) < 0.0)
                    continue;

                // Calculate angle between up vector and direction to sample
                float angleToSample = atan(v.z / length(v.xy));

                // Update maximum horizon angle and vector
                if (angleToSample > maxHorizonAngle) {
                    maxHorizonAngle = angleToSample;
                    horizonVec = v;
                }
            }

            vec3 leftDirection = cross(normalize(viewPos), vec3(dir,0));
            vec3 projectedNormal = viewNormal - dot(leftDirection, viewNormal) * leftDirection;
            float projectedLen = length(projectedNormal);
            projectedNormal /= projectedLen;

            vec3 tangent = cross(projectedNormal, leftDirection);

            const float bias = (3.141592/360)*20.f;

            float tangentAngle = atan(tangent.z / length(tangent.xy));

            // Calculate AO contribution for this direction
            float horizonAngle = max(0.0, maxHorizonAngle);
            float aoContribution = sin(horizonAngle) - sin(tangentAngle + bias);
            ao += aoContribution;
        }

        // Average AO over all directions
        ao = 1.0 - ao / float(n_dirs);

        texColor = vec4(vec3(ao), 1.0);
    }
    frag_color = texColor;
}
