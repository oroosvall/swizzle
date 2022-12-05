#ifndef SCENE_SETTINGS_HPP
#define SCENE_SETTINGS_HPP

#include <common/Common.hpp>

struct SceneRenderSettings
{
    SwBool mEnableNormalMaps;
    SwBool mEnableAnimatedTextures;
    SwBool mHeightMap;
    SwBool mTesselation;
};

#endif