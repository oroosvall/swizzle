#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 glowColor;

layout(set=0, binding=0) uniform sampler2D diffuseTexture;
layout(set=0, binding=1) uniform sampler2D glowTexture;

void main()
{
    fragColor = texture(diffuseTexture, uv);
    glowColor = texture(glowTexture, uv);
    // glowColor = vec4(1.0);
}