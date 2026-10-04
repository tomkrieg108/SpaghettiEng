#version 420 core

in vec4 v_color;
out vec4 v_frag_color;

void main()
{
	v_frag_color = v_color;
};