#version 450

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 worldPos;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec4 v_eye;
layout(location = 4) in vec3 v_center;

layout(location = 0) out vec4 fragColor;

layout(set=0, binding=0) uniform samplerCube tex;

layout(set=0, binding=1) uniform Light
{
    vec4 lightColor;
} lamp;

void main()
{
    
    vec3 viewDir = normalize(v_eye - worldPos).xyz;
    vec3 cameraLoc = v_eye.xyz;
    //vec3 


    //fragColor = vec4(viewDir, 1.0);
    fragColor = vec4(texture(tex, viewDir.xyz).rgb, 1.0);
}