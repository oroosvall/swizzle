#ifndef GAME_HPP
#define GAME_HPP

#include "Camera.hpp"
#include "CameraController.hpp"
#include <swizzle/Swizzle.hpp>

#include <vector>

#include <utils/Config.hpp>
#include <utils/FpsCounter.hpp>

#include <scene/Scene.hpp>

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

    void imGuiRender(common::Unique<sw::gfx::CommandTransaction>& trans);

    std::string getStatisticsText() const;

    common::Resource<sw::gfx::CommandBuffer> mCmdBuffer;
    common::Resource<sw::gfx::FrameBuffer> mFrameBuffer;

    PerspectiveCamera cam;
    CameraController mController;

    utils::FpsCounter mFpsCounter;

    common::Resource<sw::gfx::Shader> mFsq;
    common::Resource<sw::gfx::Material> mFsqMat;
    common::Resource<Compositor> mCompositor;
    common::Resource<Scene> mScene;

    SwBool mInputLocked;

    S32 mSelectedOption = 0;
    std::vector<sw::gfx::VSyncTypes> mVSyncOptions;
};

#endif