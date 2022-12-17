#version 450
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout (location = 0) out vec3 outPos;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec3 norm;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
    vec4 eye;
} cam;

void main() 
{

    vec3 centerPos = gl_in[0].gl_Position.xyz;
    vec3 toCamera = normalize(cam.eye.xyz - centerPos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up);

    // the vector to camera is also the normal
    norm = toCamera;

    vec3 outPos = centerPos - vec3(right * 0.5);
    
    outPos.y -= 0.5;
    uv = vec2(0.0, 0.0);
    gl_Position = cam.viewProj * vec4(outPos, 1.0);
    norm = toCamera;
    EmitVertex();

    outPos.y += 1.0;
    uv = vec2(0.0, 1.0);
    gl_Position = cam.viewProj * vec4(outPos, 1.0);
    norm = toCamera;
    EmitVertex();

    outPos.y -= 1.0;
    outPos += right;
    uv = vec2(1.0, 0.0);
    gl_Position = cam.viewProj * vec4(outPos, 1.0);
    norm = toCamera;
    EmitVertex();

    outPos.y += 1.0;
    uv = vec2(1.0, 1.0);
    gl_Position = cam.viewProj * vec4(outPos, 1.0);
    norm = toCamera;
    EmitVertex();

    EndPrimitive();
}