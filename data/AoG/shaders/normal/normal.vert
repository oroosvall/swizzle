#version 450

layout (location = 0) in vec3 vertPos;

layout (location = 1) in mat4 mdlMat;

layout (location = 0) out vec3 worldPos;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

void main() 
{
    vec4 wpos = mdlMat * vec4(vertPos, 1.0);
    worldPos = wpos.xyz;
    gl_Position = cam.viewProj * wpos;
}