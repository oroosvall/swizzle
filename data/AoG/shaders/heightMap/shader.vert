#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;

layout (location = 3) in mat4 mdlMat;

layout (location = 0) out vec3 worldPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outNormals;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

void main() 
{
    vec4 wpos = mdlMat * vec4(vertPos, 1.0);
    worldPos = wpos.xyz;
    outNormals = (mdlMat * vec4(normals, 1.0)).xyz;

    gl_Position = cam.viewProj * wpos;
    outUV = uvs;
}