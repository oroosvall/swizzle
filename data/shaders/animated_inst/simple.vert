#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;
layout (location = 3) in uvec4 boneIndex;
layout (location = 4) in vec4 boneWeight;

layout (location = 5) in mat4 mdlMat;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 worldPos;
layout (location = 2) out vec4 wgt;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

uniform layout (binding = 1, set = 0) ANIM
{
    mat4 bones[200];
} skel;

void main() 
{
    vec4 vPos = vec4(0.0);

    vec4 pos = vec4(vertPos, 1.0);

    mat4 trans = skel.bones[(boneIndex[0])];
    vec4 vTrans = trans * pos;
    vPos += (vTrans * boneWeight[0]);
    
    trans = skel.bones[(boneIndex[1])];
    vTrans = trans * pos;
    vPos += (vTrans * boneWeight[1]);

    trans = skel.bones[(boneIndex[2])];
    vTrans = trans * pos;
    vPos += (vTrans * boneWeight[2]);

    trans = skel.bones[(boneIndex[3])];
    vTrans = trans * pos;
    vPos += (vTrans * boneWeight[3]);

    //vec3 p = vPos.xyz;

    worldPos = mdlMat * vPos;

    gl_Position = cam.viewProj * worldPos;
    outUV = uvs;
}