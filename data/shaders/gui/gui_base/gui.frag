#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

layout(set=0, binding=0) uniform sampler2DArray tex;

void main()
{
    fragColor = texture(tex, vec3(uv, 1.0));
}