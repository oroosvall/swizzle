#ifndef DAY_OPTIONS_HPP
#define DAY_OPTIONS_HPP

#include <common/Common.hpp>
#include <scene/SceneSettings.hpp>
#include <glm/glm.hpp>

struct DayOptions
{
    SwBool mShowShaderEditor;
    SwBool mEnableDof;
    F32 mDoFFocalPoint;
    F32 mDoFFocalScale;
    SwBool mGlow;
    SwBool mHasMeshShaderSupport;
    SwBool mBesierCurves;
    SwBool mBesierCurvesEditor;
    SwBool mCameraPath;
    F32 mCameraTime;
    SwBool mDithering;
    F32 mSkyTime;
    SwBool mCountSkyTime;
    SwBool mLensFlare;
    glm::vec2 mFlarePos;
    SwBool mTextureViewer;
    SwBool mSSAO;
};

void DrawDayOptions(SceneRenderSettings& settings, DayOptions& opt);


#endif