#ifndef SCENE_SETTINGS_HPP
#define SCENE_SETTINGS_HPP

#include <common/Common.hpp>

#include <glm/glm.hpp>

struct SkyInfo
{
    glm::mat4 mCameraMatrix;
    glm::vec4 mCameraEye;
    glm::vec4 mSunMoonDir;
    glm::vec4 mDaySkyColor;
    glm::vec4 mNightSkyColor;
    glm::vec4 mSun1Color;
    glm::vec4 mSun2Color;
    glm::vec4 mMoon1Color;
    glm::vec4 mMoon2Color;
};

struct SceneRenderSettings
{
    SwBool mEnableNormalMaps;
    SwBool mEnableAnimatedTextures;
    SwBool mHeightMap;
    SwBool mTesselation;
    SwBool mParticles;
    SwBool mMipMaps;
    SwBool mForceLowestMipLayer;
    SwBool mMeshShaders;
    SkyInfo mSkyInfo;
};

struct ViewProjection
{
    glm::mat4 view;
    glm::mat4 proj;
};

#endif