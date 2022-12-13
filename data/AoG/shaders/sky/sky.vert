#version 450

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec2 vertUv;
layout(location = 3) in mat4 mdlMat;

layout(location = 0) out vec4 worldPos;
layout(location = 1) out vec4 worldPos2;

uniform layout (binding = 0, set = 0) SETT
{
    mat4 mCameraMatrix;
    vec4 mCameraEye;
    vec4 mSunMoonDir;
    vec4 mDaySkyColor;
    vec4 mNightSkyColor;
    vec4 mSun1Color;
    vec4 mSun2Color;
    vec4 mMoon1Color;
    vec4 mMoon2Color;
} settings;

void main() 
{
    worldPos =  vec4(vertPos, 1.0);
    worldPos2 = mdlMat * worldPos;
    gl_Position = settings.mCameraMatrix * worldPos2;
}