#version 460 core

layout (location = 0) in vec2 aPos;

out vec2 uv;

uniform vec2 position;
uniform float radius;

void main()
{
    uv = aPos;
    gl_Position = vec4(position + aPos * radius, 0.0, 1.0);
}