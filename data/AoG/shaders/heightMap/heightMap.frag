#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 glowColor;
layout(location = 2) out vec4 normalColor;
layout(location = 3) out vec4 worldColor;

layout(set=0, binding=1) uniform sampler2D tex;

float linearDepth(float depth, float near, float far)
{
    float z = depth * 2.0f - 1.0f; 
    return (2.0f * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 color = texture(tex, vec2(uv.s, 1-uv.t));
    fragColor = color;
    glowColor = vec4(0.0);

    normalColor = vec4(normalize(norm) * 0.5 + 0.5, 1.0);
    //normalColor = vec4(normalize(norm), 1.0);
    //normalColor.y = 0.0;
    worldColor = vec4(worldPos, linearDepth(gl_FragCoord.z, 0.01, 100.0));
}