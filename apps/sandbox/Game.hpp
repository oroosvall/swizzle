#ifndef GAME_HPP
#define GAME_HPP

#include <swizzle/Swizzle.hpp>
#include "Camera.hpp"
#include "CameraController.hpp"

#include <vector>

#include "Model.hpp"

#include <utils/Config.hpp>
#include <utils/FpsCounter.hpp>

#include <swizzle/gui/GuiLabel.hpp>

#include <swizzle/asset/MeshLoader.hpp>

class Game : public sw::Application
{
public:

    Game();
    ~Game();

    virtual void userSetup() override;

    virtual SwBool userUpdate(F32 dt) override;
    virtual void userCleanup() override;

private:

    void updateMainWindow();

    common::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    common::Resource<sw::gfx::Shader> mShader;
    common::Resource<sw::gfx::Material> mMaterial;

    common::Resource<sw::gfx::Texture> mTexture;

    common::Resource<sw::gfx::FrameBuffer> mFrameBuffer;

    common::Resource<sw::gfx::Buffer> mUniformBuffer;

    sw::gfx::RGBA rgba = { 0.4F, 0.17F, 0.05F, 1.0F };
    sw::gfx::RGBA lampColor = { 0.8F, 0.2F, 0.2F, 1.0F };
    PerspectiveCamera cam;
    CameraController mController;

    sw::Mesh mMesh;
    sw::Mesh mSkysphere;

    common::Resource<sw::gfx::Shader> mSkyShader;

    common::Resource<sw::gfx::Texture> mSkyTexture;
    common::Resource<sw::gfx::Material> mSkyMaterial;

    sw::MeshAnimated mMeshAnimated;
    common::Resource<sw::gfx::Shader> mAnimationShader;
    common::Resource<sw::gfx::Material> mAnimationMaterial;

    U8 singlePixel[16] = { 0U, 0U, 0U, 255U, 128, 0, 0, 255, 0, 128, 0, 255, 0, 0, 128, 255 };

    utils::Config mGameCfg;
    utils::FpsCounter mFpsCounter;

    // Temp
    sw::gui::GuiLabel* guiLabel;
    common::Resource<sw::gfx::Shader> mTextShader;
    common::Resource<sw::gfx::Material> mTextMaterial;

    int extraText = 0;

    int selectedBone = 0;

    bool testMode = false;

};

#endif