#version 450
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec3 Position[3];
layout (location = 1) in vec2 UVCord[3];
layout (location = 2) in vec3 NormalIn[3];

layout (location = 0) out vec3 PositionOut;
layout (location = 1) out vec2 UVOut;
layout (location = 2) out vec3 norm;
layout (location = 3) out mat3 TBNOut;

void main() 
{

    vec3 v0 = Position[0];
    vec3 v1 = Position[1];
    vec3 v2 = Position[2];

    // Shortcuts for UVs
    vec2 uv0 = UVCord[0];
    vec2 uv1 = UVCord[1];
    vec2 uv2 = UVCord[2];
    
    // Edges of the triangle : postion delta
    vec3 deltaPos1 = v1-v0;
    vec3 deltaPos2 = v2-v0;
    
    // UV delta
    vec2 deltaUV1 = uv1-uv0;
    vec2 deltaUV2 = uv2-uv0;
    
    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    vec3 tangent = normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);

    PositionOut = Position[0];
    gl_Position = gl_in[0].gl_Position;
    UVOut = UVCord[0];
    TBNOut = mat3(normalize(tangent - dot(tangent, NormalIn[0]) * NormalIn[0]), cross(tangent, NormalIn[0]), NormalIn[0]);
    norm = NormalIn[0];
    EmitVertex();

    PositionOut = Position[1];
    gl_Position = gl_in[1].gl_Position;
    UVOut = UVCord[1];
    TBNOut = mat3(normalize(tangent - dot(tangent, NormalIn[1]) * NormalIn[1]), cross(tangent, NormalIn[1]), NormalIn[1]);
    norm = NormalIn[1];
    EmitVertex();
        
    PositionOut = Position[2];
    gl_Position = gl_in[2].gl_Position;
    UVOut = UVCord[2];
    TBNOut = mat3(normalize(tangent - dot(tangent, NormalIn[2]) * NormalIn[2]), cross(tangent, NormalIn[2]), NormalIn[2]);
    norm = NormalIn[2];
    EmitVertex();		

    EndPrimitive();
}