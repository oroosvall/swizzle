#version 450

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 glowColor;
layout(location = 2) out vec4 normalColor;
layout(location = 3) out vec4 worldColor;


uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec3 col;
} cam;

void main()
{
    fragColor = vec4(cam.col, 1.0);
    glowColor = vec4(0.0);
    normalColor = vec4(0.0);
    worldColor = vec4(0.0);
}