#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 fragColor;

layout(set=0, binding=0) uniform sampler2D normalTexture;

vec4 CalcBumpedNormal()
{
    vec3 normalMap = texture(normalTexture, vec2(uv.s, 1-uv.t)).rgb;
    normalMap *= 2.2; // gamma correction
    vec3 newnormal = (2.0 * normalMap) - 1.0;
    newnormal = TBN * newnormal;
    newnormal = normalize(newnormal);
    return vec4(newnormal, 1.0);
}

void main()
{
    vec4 normalMap = CalcBumpedNormal();
    fragColor = vec4(normalMap.xyz, 1.0);
}