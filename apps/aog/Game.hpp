#ifndef GAME_HPP
#define GAME_HPP

#include "Camera.hpp"
#include "CameraController.hpp"
#include <swizzle/Swizzle.hpp>

#include <vector>

#include <utils/Config.hpp>
#include <utils/FpsCounter.hpp>

#include <scene/Scene.hpp>

#include "imgui/ShaderEditor.hpp"

#include <Bezier.hpp>

class Game : public sw::Application
{
public:
    Game();
    ~Game();

    virtual void userSetup() override;

    virtual SwBool userUpdate(F32 dt) override;
    virtual void userCleanup() override;

private:
    void updateMainWindow(F32 dt);

    common::Resource<ShaderEditor> mShaderEditor;

    void imGuiRender(common::Unique<sw::gfx::CommandTransaction>& trans);

    std::string getStatisticsText() const;

    common::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    common::Resource<sw::gfx::FrameBuffer> mGBuffer;

    PerspectiveCamera cam;
    CameraController mController;

    utils::FpsCounter mFpsCounter;

    common::Resource<sw::gfx::Shader> mFsq;
    common::Resource<sw::gfx::Material> mFsqMat;
    common::Resource<Compositor> mCompositor;
    common::Resource<AssetManager> mAssetManager;
    common::Resource<Scene> mScene;

    SceneRenderSettings mSceneSettings;
    SwBool mShowShaderEditor;

    SwBool mInputLocked;

    S32 mSelectedOption = 0;
    std::vector<sw::gfx::VSyncTypes> mVSyncOptions;

    SwBool mEnableDof = false;
    F32 mDoFFocalPoint = 40.0f;
    F32 mDoFFocalScale = 200.0f;

    SwBool mGlow = true;

    SwBool mBesierCurves = true;
    SwBool mBesierCurvesEditor = false;
    U32 mSelectedCurve = 0u;
    U32 mBezierItem = 0u;

    common::Resource<sw::gfx::Shader> mBezierShader;

    std::vector<BezierCurve> mCurves;
    BezierCurve* getCurrentCurve(U32 index);


    U32 mCameraPathIndex = 0u;
    U32 mCameraLookIndex = 0u;
    SwBool mCameraPath = false;
    F32 mCameraTime = 0.0f;

    U32 comboIdx = 0;
    void imguiCurveComboSelect(U32* curveIndex);

    void updateSkyTime();
    F32 mTime;
    F32 mSkyCycleTime;
    F32 mSkyTime;
    SwBool mCountSkyTime = true;
    SwBool mDithering = true;

};

#endif
