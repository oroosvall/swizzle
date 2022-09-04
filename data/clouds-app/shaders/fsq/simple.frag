#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

layout(set=0, binding=0) uniform sampler2D diffuse;
layout(set=0, binding=1) uniform sampler2D normal;
layout(set=0, binding=2) uniform sampler2D worldPos;
layout(set=0, binding=3) uniform sampler2D imgui;

layout(push_constant) uniform Camera
{
    vec4 eye; // w contains fog distance
    uint flags;
} cam;

vec4 SRGBToLinear(vec4 c)
{
    return vec4(pow(c.rgb, vec3(2.4)), c.a);
}

vec4 doFog(vec4 diffuse, vec4 fogColor, vec3 eye, vec3 pos)
{
    float dist = distance(eye, pos);

    dist = clamp(0.0, 50.0, dist);
    float fogFactor = clamp(0.0, 1.0, dist / cam.eye.w);

    diffuse.rgb = mix(diffuse.rgb, SRGBToLinear(fogColor).rgb, fogFactor);
    diffuse.a = 1.0;
    return diffuse;
}

void main()
{
    vec4 world = texture(worldPos, uv);

    vec4 diffuseColor = texture(diffuse, uv);
    vec4 guiColor = SRGBToLinear(texture(imgui, uv));

    if (bitfieldExtract(cam.flags, 0, 1) == 1)
    {
        diffuseColor = doFog(diffuseColor, vec4(0.4), cam.eye.xyz, world.xyz);
    }

    diffuseColor.rgb = clamp(vec3(0.0), vec3(1.0), diffuseColor.rgb);

    vec4 col = mix(diffuseColor, guiColor, guiColor.a);

    fragColor = col;
}