#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

layout(set=0, binding=0) uniform sampler2D diffuseTexture;

vec4 SRGBToLinear(vec4 c)
{
    return vec4(pow(c.rgb, vec3(2.4)), c.a);
}

void main()
{
    fragColor = SRGBToLinear(texture(diffuseTexture, uv));
}