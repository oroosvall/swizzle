#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 1) in ivec4 boneIndex;
layout (location = 2) in vec4 boneWeight;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 worldPos;

uniform layout( push_constant) Push
{
    mat4 mvp;
} push;

uniform layout (binding = 0, set = 0) ANIM
{
    mat4 bones[200];
} skel;

void main() 
{

    vec4 vPos = vec4(0.0);

    mat4 trans = skel.bones[(boneIndex[0])];
    vec4 vTrans = trans * vec4(vertPos, 1.0);
    vPos += (vTrans * boneWeight[0]);
    
    trans = skel.bones[(boneIndex[1])];
    vTrans = trans * vec4(vertPos, 1.0);
    vPos += (vTrans * boneWeight[1]);

    trans = skel.bones[(boneIndex[2])];
    vTrans = trans * vec4(vertPos, 1.0);
    vPos += (vTrans * boneWeight[2]);

    trans = skel.bones[(boneIndex[3])];
    vTrans = trans * vec4(vertPos, 1.0);
    vPos += (vTrans * boneWeight[3]);

    gl_Position = push.mvp * vPos;
    worldPos = vPos;
    outUV = vec2(gl_Position.xy * 2.0);
}