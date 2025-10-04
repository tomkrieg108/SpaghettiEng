#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;

uniform mat4 u_model;
uniform mat4 u_proj;
uniform mat4 u_view;

out vec4 v_color;

void main()
{
	gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
	v_color = a_color;
};