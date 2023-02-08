#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;

uniform layout(push_constant) Camera
{
    mat4 viewProj;
} cam;

void main() 
{
    gl_Position = cam.viewProj * vec4(pos, 1.0);
    outColor = color;
}