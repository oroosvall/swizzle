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

    sw::core::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    sw::core::Resource<sw::gfx::Shader> mShader;
    sw::core::Resource<sw::gfx::Material> mMaterial;

    sw::core::Resource<sw::gfx::Texture> mTexture;

    sw::core::Resource<sw::gfx::FrameBuffer> mFrameBuffer;

    sw::core::Resource<sw::gfx::Buffer> mUniformBuffer;

    sw::gfx::RGBA rgba = { 0.4F, 0.17F, 0.05F, 1.0F };
    sw::gfx::RGBA lampColor = { 0.8F, 0.2F, 0.2F, 1.0F };
    PerspectiveCamera cam;
    CameraController mController;

    sw::Mesh mMesh;
    sw::Mesh mSkysphere;

    sw::core::Resource<sw::gfx::Shader> mSkyShader;

    sw::core::Resource<sw::gfx::Texture> mSkyTexture;
    sw::core::Resource<sw::gfx::Material> mSkyMaterial;

    U8 singlePixel[16] = { 0U, 0U, 0U, 255U, 128, 0, 0, 255, 0, 128, 0, 255, 0, 0, 128, 255 };

    utils::Config mGameCfg;
    utils::FpsCounter mFpsCounter;

    // Temp
    sw::gui::GuiLabel* guiLabel;
    sw::core::Resource<sw::gfx::Shader> mTextShader;
    sw::core::Resource<sw::gfx::Material> mTextMaterial;

    int extraText = 0;

};

#endif