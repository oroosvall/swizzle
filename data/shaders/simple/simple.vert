#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 worldPos;

uniform layout( push_constant) Camera
{
    mat4 mvp;
} cam;

void main() 
{
    //vec3 p = vertPos;
    //p.y = -p.y;
    gl_Position = cam.mvp * vec4(vertPos, 1.0);
    worldPos = gl_Position;
    outUV = vec2(gl_Position.xy * 2.0);
    //outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    //gl_Position = vec4(outUV * 2.0f + -1.0f, 0.0f, 1.0f);
}