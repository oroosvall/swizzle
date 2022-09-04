#version 450

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec4 vertexColor;

layout(location = 3) in mat4 mdlMat;

layout(location = 0) out vec4 worldPos;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec4 v_color;
layout(location = 3) out vec4 v_eye;


uniform layout (binding = 0, set = 0) CAM
{
    mat4 cam;
    vec4 eye;
} camera;

void main() 
{
    worldPos =  mdlMat * vec4(vertPos, 1.0);
    gl_Position = camera.cam * worldPos;
    v_normal = (mdlMat * vec4(vertNorm, 0.0)).xyz;

    v_color = vertexColor;
    v_eye = camera.eye;
}