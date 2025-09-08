#version 460 core

layout (location = 0) in vec2 aPos;

uniform float aspectRatio;

void main()
{
    vec2 screenPos = aPos;
    screenPos.x /= aspectRatio;
    gl_Position = vec4(screenPos, 0.0, 1.0);
}