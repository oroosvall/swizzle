#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUv;

layout (location = 0) out vec2 outUV;

uniform layout( push_constant) Camera
{
    mat4 viewProjection;
    vec3 offset;
} cam;

void main() 
{
    gl_Position = cam.viewProjection * vec4(vertPos + cam.offset, 1.0);
    outUV = vertUv;
}