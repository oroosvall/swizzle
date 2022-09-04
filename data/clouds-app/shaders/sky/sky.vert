#version 450

layout(location = 0) in vec3 vertPos;

layout(location = 1) in mat4 mdlMat;

layout(location = 0) out vec4 worldPos;
layout(location = 1) out vec4 worldPos2;

uniform layout (binding = 0, set = 0) CAM
{
    mat4 cam;
    vec4 eye;
} camera;

void main() 
{
    worldPos =  vec4(vertPos, 1.0);
    worldPos2 = mdlMat * worldPos;
    gl_Position = camera.cam * worldPos2;
}