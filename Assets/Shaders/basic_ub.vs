#version 420 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;

// Per frame data
// layout(set = 0, binding = 0) in Vulkan
layout(std140, binding = 0) uniform FrameUB {
    //float total_time;
    //vec4 ambient_light;
    vec3 light_pos;
    vec3 light_colour;
};

//Per pass data
// layout(set = 1, binding = 0) in Vulkan
layout(std140, binding = 1) uniform PassUB {
    mat4 view;
    mat4 proj;
    vec3 camera_position;
};

//Per model data
uniform mat4 u_model;

out vec4 v_color;

void main()
{
	gl_Position = proj * view * u_model * vec4(a_position, 1.0);
	v_color = a_color;
};