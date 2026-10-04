#version 420 core

in vec3 normal;  
in vec3 frag_pos; 

out vec4 frag_color;

// Per frame data
// layout(set = 0, binding = 0) in Vulkan
layout(std140, binding = 0) uniform FrameUB {
    vec3 light_pos;
    vec3 light_colour;
    //float total_time;
    //vec4 ambient_light;
};

//Per pass data
// layout(set = 1, binding = 0) in Vulkan
layout(std140, binding = 1) uniform PassUB {
    mat4 view;
    mat4 proj;
    vec3 camera_position;
};

uniform vec3 u_object_colour;

void main()
{
  // ambient
  float ambient_strength = 0.1;
  vec3 ambient = ambient_strength * light_colour;

  // diffuse 
  vec3 norm = normalize(normal);
  vec3 ligh_dir = normalize(light_pos - frag_pos);
  float diff = max(dot(norm, ligh_dir), 0.0);
  vec3 diffuse = diff * light_colour;

  // specular
  float specular_strength = 0.3;
  vec3 view_dir = normalize(camera_position - frag_pos);
  vec3 reflect_dir = reflect(-ligh_dir, norm);  
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  vec3 specular = specular_strength * spec * light_colour;  
      
  vec3 result = (ambient + diffuse + specular) * u_object_colour;
  result =clamp(result, 0.0,1.0);
  frag_color = vec4(result, 1.0);
} 