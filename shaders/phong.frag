#version 330

out vec4 frag_color;
in vec3 normal_ws;
in vec3 frag_ws;
uniform mat4 view;


uniform vec3 light;

struct Material {
    float k_s;
    vec3 col;
};

struct Light {
    float light_intensity;
    float k_a;
    vec3 col;
    vec3 pos_ws;
};

vec3 phong(Material mat, Light light){
    vec3 incidence_vec = normalize(light.pos_ws - frag_ws);
    vec3 normalized_normal = normalize(normal_ws);


    vec3 camera_ws = vec3(inverse(view) * vec4(0,0,0,1));
    vec3 view_dir = normalize(camera_ws - frag_ws);
    vec3 reflect_vec = reflect(-incidence_vec, normalized_normal);

    float ambient = light.k_a;
    float diffuse = clamp(dot(normalized_normal, incidence_vec), 0, 1);
    float specular = pow(clamp(dot(view_dir, reflect_vec), 0, 1), mat.k_s);


    return light.light_intensity * light.col * (ambient + diffuse + specular);
}


void main (void) {

    Material mat;
    mat.col = vec3(1,1,1);
    mat.k_s = 10;

    Light l;
    l.col = vec3(1,1,1);
    l.k_a = 0.4;
    l.pos_ws = light;
    l.light_intensity = 0.5;

    frag_color = vec4(mat.col * phong(mat, l), 1);
}
