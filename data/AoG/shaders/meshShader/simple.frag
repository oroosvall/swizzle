#version 450
#extension GL_EXT_mesh_shader : enable

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 glowColor;

layout (location=0) perprimitiveEXT in flat uint workgroup;

void main()
{
    uint wg = (workgroup % 128); // wrap at 128
    float v = (float(wg) / 255.0) + 0.5;
    fragColor = vec4(v);
    glowColor = vec4(0.0);
}