#version 450

layout (location = 0) in vec3 vertPos;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

void main() 
{
    gl_Position = cam.viewProj * vec4(vertPos, 1.0);
}