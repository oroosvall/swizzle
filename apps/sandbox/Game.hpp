#ifndef GAME_HPP
#define GAME_HPP

#include <swizzle/Swizzle.hpp>
#include "Camera.hpp"
#include "CameraController.hpp"

#include <vector>

#include "Model.hpp"

#include <utils/Config.hpp>
#include <utils/FpsCounter.hpp>

#include "assetManager/AssetManager.hpp"
#include "scene/Scene.hpp"

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

    void imGuiRender();

    common::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    common::Resource<sw::gfx::FrameBuffer> mFrameBuffer;

    PerspectiveCamera cam;
    CameraController mController;

    utils::Config mGameCfg;
    utils::FpsCounter mFpsCounter;

    common::Resource<sw::gfx::Shader> mFsq;
    common::Resource<sw::gfx::Material> mFsqMat;

    common::Resource<AssetManager> mAssetManager;
    common::Resource<Compositor> mCompositor;
    common::Resource<Scene> mScene;

};

#endif