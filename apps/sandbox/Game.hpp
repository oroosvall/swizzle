#ifndef GAME_HPP
#define GAME_HPP

#include "Camera.hpp"
#include "CameraController.hpp"
#include <swizzle/Swizzle.hpp>
#include <swizzle/ApplicationTemplate.hpp>

#include <vector>

#include <utils/Config.hpp>
#include <utils/FpsCounter.hpp>

#include "assetManager/AssetManager.hpp"
#include "scene/Scene.hpp"

#include <ImGuiSwzzle.hpp>

class Game : public sw::Application
{
public:
    Game(const SwChar* appName);
    virtual ~Game();

    virtual void userSetup() override;

    virtual SwBool userUpdate(F32 dt) override;
    virtual void userCleanup() override;

private:
    void updateMainWindow(F32 dt);

    void imGuiRender(common::Unique<sw::gfx::CommandTransaction>& trans);

    common::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    PerspectiveCamera cam;
    CameraController mController;

    utils::Config mGameCfg;
    utils::FpsCounter mFpsCounter;

    common::Resource<sw::gfx::Shader> mFsq;
    common::Resource<sw::gfx::Material> mFsqMat;

    common::Resource<AssetManager> mAssetManager;
    common::Resource<Compositor> mCompositor;
    common::Resource<Scene> mScene;
    common::Resource<ImGuiSwizzleRenderTarget> mImGuiRenderTarget;
    common::Resource<sw::gfx::FrameBuffer> mImGuiFbo;

    U32 mLastWidth;
    U32 mLastHeight;
};

#endif