#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 uv;

layout (location = 2) in mat4 mdlMat;

layout (location = 0) out vec3 worldPos;

layout(set=0, binding=1) uniform sampler2D height1;
layout(set=0, binding=2) uniform sampler2D height2;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

void main() 
{
    vec4 vPos = vec4(vertPos, 1.0);
    if (cam.eye.w > 0.5)
    {
        vPos.y = texture(height1, uv).r;
    }
    else
    {
        vPos.y = texture(height2, uv).r;
    }
    vec4 wpos = mdlMat * vPos;
    worldPos = wpos.xyz;
    gl_Position = cam.viewProj * wpos;
}