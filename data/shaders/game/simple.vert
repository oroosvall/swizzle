#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNorm;
layout (location = 2) in vec2 vertUv;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 worldPos;
layout (location = 2) out vec3 v_normal;
layout (location = 3) out vec4 v_eye;

uniform layout( push_constant) Camera
{
    mat4 model;
    mat4 view;
    mat4 projection;
    vec4 eye;
} cam;

void main() 
{
    //worldPos = cam.view * cam.model * vec4(vertPos, 1.0);
    //gl_Position = cam.projection * worldPos;
    worldPos =  cam.model * vec4(vertPos, 1.0);
    gl_Position = cam.projection * cam.view * worldPos;
    
    //v_normal = (transpose(inverse(cam.view * cam.model)) * vec4(vertNorm, 0.0)).xyz;
    v_normal = (cam.model * vec4(vertNorm, 0.0)).xyz;
    v_eye = cam.eye;

    outUV = vertUv; //vec2(gl_Position.xy * 2.0);
}