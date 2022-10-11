#shader vertex
#version 330 core
layout(location = 0) in vec3 a_Pos;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_CameraMatrix * vec4(u_ModelMatrix * vec4(a_Pos, 1.0f));
}

#shader fragment
#version 330 core

out vec4 a_color;

uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;

uniform float focal_length;
uniform float aspect_ratio;
uniform vec2 viewport_size;
uniform vec3 ray_origin;
uniform vec3 top;
uniform vec3 bottom;

uniform vec3 background_colour;
uniform vec3 material_colour;
uniform vec3 light_position;

uniform float step_length;
uniform float threshold;

uniform sampler3D volume;
// uniform sampler2D jitter;

uniform float gamma;



// Ray
struct Ray {
    vec3 origin;
    vec3 direction;
};

// Axis-aligned bounding box
struct AABB {
    vec3 top;
    vec3 bottom;
};

// Estimate normal from a finite difference approximation of the gradient
vec3 normal(vec3 position, float intensity)
{
    float d = step_length;
    float dx = texture(volume, position + vec3(d,0,0)).r - intensity;
    float dy = texture(volume, position + vec3(0,d,0)).r - intensity;
    float dz = texture(volume, position + vec3(0,0,d)).r - intensity;
    return -normalize(NormalMatrix * vec3(dx, dy, dz));
}

// Slab method for ray-box intersection
void ray_box_intersection(Ray ray, AABB box, out float t_0, out float t_1)
{
    vec3 direction_inv = 1.0 / ray.direction;
    vec3 t_top = direction_inv * (box.top - ray.origin);
    vec3 t_bottom = direction_inv * (box.bottom - ray.origin);
    vec3 t_min = min(t_top, t_bottom);
    vec2 t = max(t_min.xx, t_min.yz);
    t_0 = max(0.0, max(t.x, t.y));
    vec3 t_max = max(t_top, t_bottom);
    t = min(t_max.xx, t_max.yz);
    t_1 = min(t.x, t.y);
}

// A very simple colour transfer function
vec4 colour_transfer(float intensity)
{
    vec3 high = vec3(1.0, 1.0, 1.0);
    vec3 low = vec3(0.0, 0.0, 0.0);
    float alpha = (exp(intensity) - 1.0) / (exp(1.0) - 1.0);
    return vec4(intensity * high + (1.0 - intensity) * low, alpha);
}

void main()
{
    vec3 ray_direction;
    ray_direction.xy = 2.0 * gl_FragCoord.xy / viewport_size - 1.0;
    ray_direction.x *= aspect_ratio;
    ray_direction.z = -focal_length;
    ray_direction = (vec4(ray_direction, 0) * ViewMatrix).xyz;

    float t_0, t_1;
    Ray casting_ray = Ray(ray_origin, ray_direction);
    AABB bounding_box = AABB(top, bottom);
    ray_box_intersection(casting_ray, bounding_box, t_0, t_1);

    vec3 ray_start = (ray_origin + ray_direction * t_0 - bottom) / (top - bottom);
    vec3 ray_stop = (ray_origin + ray_direction * t_1 - bottom) / (top - bottom);

    vec3 ray = ray_stop - ray_start;
    float ray_length = length(ray);
    vec3 step_vector = step_length * ray / ray_length;

    // Random jitter
    //ray_start += step_vector * texture(jitter, gl_FragCoord.xy / viewport_size).r;

    vec3 position = ray_start;

    float maximum_intensity = 0.0;

    // Ray march until reaching the end of the volume
    while (ray_length > 0) {

        float intensity = texture(volume, position).r;

        /*
        if (position.x < 0 || position.y < 0 || position.z < 0)
        {
            a_color = vec4(1,0,0,1);
            return;
        }
        

        if (position.x > 1 || position.y > 1 || position.z > 1)
        {
            a_color = vec4(0,1,0,1);
            return;
        }
        */

        if (intensity > maximum_intensity) {
            maximum_intensity = intensity;
        }

        ray_length -= step_length;
        position += step_vector;
    }

    vec4 colour = colour_transfer(maximum_intensity);

    // Blend background
    a_color.rgb = colour.a * colour.rgb + (1 - colour.a) * pow(background_colour, vec3(gamma)).rgb;
    a_color.a = 1.0;

    // Gamma correction
    a_color.rgb = pow(colour.rgb, vec3(1.0 / gamma));
    a_color.a = colour.a;
}