#version 330 core
layout (location = 0) in vec2 vertex; 
layout (location = 1) in vec4 color;
out vec4 Color;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex, 0.0, 1.0);
    Color = color;
} 