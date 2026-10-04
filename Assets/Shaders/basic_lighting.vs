#version 420 core
layout (location = 0) in vec3 a_position;;
layout (location = 1) in vec3 a_normal;

//Per pass data
// layout(set = 1, binding = 0) in Vulkan
layout(std140, binding = 1) uniform PassUB {
    mat4 view;
    mat4 proj;
    vec3 camera_position;
};

uniform mat4 u_model;

out vec3 frag_pos;
out vec3 normal;

void main()
{
  gl_Position = proj * view * u_model * vec4(a_position, 1.0);
  normal = mat3(transpose(inverse(u_model))) * a_normal;
    
  //frag_pos = vec3(u_model * vec4(a_position, 1.0));
  //gl_Position = proj * view * vec4(frag_pos, 1.0);
}