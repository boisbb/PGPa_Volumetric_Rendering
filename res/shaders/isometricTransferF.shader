#shader vertex
#version 420

// Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
// Author: Boris Burkalo (xburka00), 2MIT
// 
// Some parts of this shader are inspired by/taken from:
// https://github.com/jose-villegas/VolumeRendering/blob/master/VolumeRendering/shaders/raycast.frag, Author: jose-villegas
// https://martinopilia.com/posts/2018/09/17/volume-raycasting.html, Author: Martino Pilia

layout(location = 0) in vec3 a_Pos;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_CameraMatrix * vec4(u_ModelMatrix * vec4(a_Pos, 1.0f));
}

#shader fragment
#version 420

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
uniform float dither_ratio = 1.f;
uniform float pixel_offset;
uniform vec2 windowOffset;

layout(binding=0) uniform sampler3D volume;
layout(binding=1) uniform sampler1D transferFunction;

uniform float gamma = 0.4f;

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
    return normalize(NormalMatrix * vec3(dx, dy, dz));
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

void main()
{
    vec3 ray_direction;
    vec2 offsetFragCoord = vec2(gl_FragCoord.x - windowOffset.x, gl_FragCoord.y - pixel_offset - windowOffset.y);
    ray_direction.xy = 2.0 * offsetFragCoord / viewport_size - 1.0;
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

    // Dithering
    float starting_offset = t_0;
    starting_offset += step_length * fract(sin(dot(gl_FragCoord.xy, vec2(12.9898,78.233))+dither_ratio) * 43758.5453);

    ray_start += step_vector * starting_offset;// * texture(jitter, gl_FragCoord.xy / viewport_size).r;

    vec3 position = ray_start;
    // vec3 colour = pow(background_colour, vec3(gamma));
    vec3 colour = pow(background_colour, vec3(gamma));

    vec4 dst = vec4(0, 0, 0, 0);
    vec4 src = vec4(0.0f);

    // Ray march until reaching the end of the volume
    while (ray_length > 0) {
        float intensity = texture(volume, position).r;

        if (intensity > threshold) {

            // Get closer to the surface
            position -= step_vector * 0.5;
            intensity = texture(volume, position).r;

            position -= step_vector * (intensity > threshold ? 0.25 : -0.25);
            intensity = texture(volume, position).r;

            // adding the transfer funtion
            src = texture(transferFunction, intensity);

            // correcting opacity
            src.a = 1 - pow((1 - src.a), step_length / 0.5);

            vec3 light_direction = normalize(light_position - position);
            vec3 nrml = normal(position, intensity);
            vec3 lightDir = normalize(-light_direction);
            float lambert = max(dot(nrml, lightDir), 0.0);
            src.rgb = 1.0 * lambert * src.rgb + 0.1 * src.rgb;

            // blend stuff front to back
            src.rgb *= src.a;
            dst = (1.0 - dst.a) * src + dst;

            if(dst.a >= 0.95) 
                break;
        }

        ray_length -= step_length;
        position += step_vector;

        if (position.x > 1.0 || position.y > 1.0 || position.z > 1.0) 
            break;
    }

    // tone map
    vec3 mapped =  vec3(1.0) - exp(-dst.rgb * 2.0f);

    // gamma correction
    dst.rgb = pow(mapped, vec3(1.0 / gamma));
    a_color = dst;
}