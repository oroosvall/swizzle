#version 450

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;

layout (location = 3) in mat4 mdlMat;

uniform layout( push_constant) Camera
{
    mat4 viewProj;
};

void main()
{
    gl_Position = viewProj * mdlMat * vec4(vertPos, 1.0);
}