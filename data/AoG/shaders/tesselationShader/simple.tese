#version 450

layout(triangles, equal_spacing, ccw) in;

layout (location = 0) in vec3 inWorldPos[];
layout (location = 1) in vec2 inUv[];
layout (location = 2) in vec3 inNormals[];

layout (location = 0) out vec3 worldPos;
layout (location = 1) out vec2 outUv;
layout (location = 2) out vec3 outNormals;

layout(set=0, binding=1) uniform sampler2D tex;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    outUv = interpolate2D(inUv[0], inUv[1], inUv[2]);
    outNormals = normalize(interpolate3D(inNormals[0], inNormals[1], inNormals[2]));
    worldPos = interpolate3D(inWorldPos[0], inWorldPos[1], inWorldPos[2]);

    float displace = texture(tex, vec2(outUv.s, 1.0 - outUv.t)).x;
    worldPos += outNormals * displace;

    gl_Position = cam.viewProj * vec4(worldPos, 1.0);

}