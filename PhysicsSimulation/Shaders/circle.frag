#version 460 core

in vec2 uv;

out vec4 FragColor;

uniform vec3 color;

void main()
{
    float dist = dot(uv, uv);
    if (dist > 1.0)
        discard;
    
    FragColor = vec4(color, 1.0);
}
