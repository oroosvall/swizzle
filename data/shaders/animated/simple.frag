#version 450

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 worldPos;
layout(location = 2) in vec4 wgt;

layout(location = 0) out vec4 fragColor;



void main()
{
    fragColor = vec4(worldPos.xyz, 1.0);
}