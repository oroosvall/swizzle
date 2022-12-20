#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 glowColor;
layout(location = 2) out vec4 normalColor;
layout(location = 3) out vec4 worldColor;

layout(set=0, binding=1) uniform sampler2D diffuseTexture;
layout(set=0, binding=2) uniform sampler2D normalTexture;

float linearDepth(float depth, float near, float far)
{
    float z = depth * 2.0f - 1.0f; 
    return (2.0f * near * far) / (far + near - z * (far - near));
}

vec4 CalcBumpedNormal()
{
    vec3 normalMap = texture(normalTexture, vec2(uv.s, 1-uv.t)).rgb;
    normalMap *= 2.2; // gamma correction
    vec3 newnormal = normalize((2.0 * normalMap) - 1.0);
    newnormal = TBN * newnormal;
    newnormal = normalize(newnormal);
    return vec4(newnormal, 1.0);
}

void main()
{
    vec4 normalMap = CalcBumpedNormal();
    vec3 color = texture(diffuseTexture, vec2(uv.s, 1-uv.t)).rgb;
    fragColor = vec4(color, 1.0);
    glowColor = vec4(0.0);

    normalColor = vec4(normalize(normalMap.xyz) * 0.5 + 0.5, 1.0);
    worldColor = vec4(worldPos, 1.0);
}