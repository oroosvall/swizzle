#version 450
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec3 worldPos[];
layout (location = 1) in vec2 uvIn[];
layout (location = 2) in vec3 normIn[];

layout (location = 0) out vec3 outPos;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec3 norm;

void main() 
{
    vec3 normal = normIn[0];
/*normalMatrix[0] * normalize(cross(
        normalize(vec3(worldPos[1]) - vec3(worldPos[0])),
        normalize(vec3(worldPos[2]) - vec3(worldPos[0]))
    ));*/
    
    gl_Position = gl_in[0].gl_Position;
    norm = normal;
    outPos = worldPos[0];
    uv = uvIn[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    norm = normal;
    outPos = worldPos[1];
    uv = uvIn[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    norm = normal;
    outPos = worldPos[2];
    uv = uvIn[2];
    EmitVertex();

    EndPrimitive();
}