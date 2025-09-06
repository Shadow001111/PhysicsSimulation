#version 460 core

layout (location = 0) in vec2 aPos;

out vec2 uv;

uniform vec2 position;
uniform float radius;

uniform float aspectRatio;

void main()
{
    uv = aPos;
    vec2 screenPos = position + aPos * radius;
    screenPos.x /= aspectRatio;
    gl_Position = vec4(screenPos, 0.0, 1.0);
}