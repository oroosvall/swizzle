#version 450
// #extension GL_EXT_mesh_shader : enable

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 glowColor;

void main()
{
    fragColor = vec4(0.5);
    glowColor = vec4(0.0);
}