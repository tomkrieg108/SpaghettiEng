#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 u_model;
uniform mat4 u_proj;
uniform mat4 u_view;

void main()
{
    gl_Position = u_proj * u_view * u_model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}