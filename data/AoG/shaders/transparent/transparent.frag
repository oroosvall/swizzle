#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;
layout(location = 3) in vec4 color;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 glowColor;
layout(location = 2) out vec4 normalColor;
layout(location = 3) out vec4 worldColor;

void main()
{
    fragColor = vec4(color);
    glowColor = vec4(0.0);

    normalColor = vec4(norm, 0.0);
    worldColor = vec4(worldPos, 0.0);
}