#version 450

layout (vertices = 3) out;

layout (location = 0) in vec3 inWorldPos[];
layout (location = 1) in vec2 inUv[];
layout (location = 2) in vec3 inNormals[];

layout (location = 0) out vec3 outWorldPos[];
layout (location = 1) out vec2 outUv[];
layout (location = 2) out vec3 outNormals[];

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

float getTesselationLevel(float distA, float distB)
{
    float avgDist = (distA + distB) / 2.0;

    if (avgDist <= 5.0)
    {
        return 25.0;
    }
    else if (avgDist <= 15.0)
    {
        return 15.0;
    }
    else
    {
        return 5.0;
    }
}

void main()
{
    outWorldPos[gl_InvocationID] = inWorldPos[gl_InvocationID];
    outUv[gl_InvocationID] = inUv[gl_InvocationID];
    outNormals[gl_InvocationID] = inNormals[gl_InvocationID];

    float EyeToVertexDistance0 = distance(cam.eye.xyz, outWorldPos[0]);
    float EyeToVertexDistance1 = distance(cam.eye.xyz, outWorldPos[1]);
    float EyeToVertexDistance2 = distance(cam.eye.xyz, outWorldPos[2]);

    gl_TessLevelOuter[0] = getTesselationLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = getTesselationLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = getTesselationLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}