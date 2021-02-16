#ifndef GAME_HPP
#define GAME_HPP

#include <swizzle/Swizzle.hpp>
#include "Camera.hpp"
#include "CameraController.hpp"

#include <vector>

#include "Model.hpp"

class Game : public sw::Application
{
public:

    Game();
    ~Game();

    virtual void userSetup() override;

    virtual SwBool userUpdate(F32 dt) override;
    virtual void userCleanup() override;

private:

    sw::core::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    sw::core::Resource<sw::gfx::Shader> mShader;

    sw::core::Resource<sw::gfx::Texture> mTexture;

    sw::core::Resource<sw::gfx::FrameBuffer> mFrameBuffer;

    sw::core::Resource<sw::gfx::Buffer> mUniformBuffer;

    sw::gfx::RGBA rgba = { 0.4F, 0.17F, 0.05F, 1.0F };
    sw::gfx::RGBA lampColor = { 0.8F, 0.2F, 0.2F, 1.0F };
    PerspectiveCamera cam;
    CameraController mController;

    std::vector<sw::core::Resource<sw::gfx::Buffer>> mBuffers;

    U8 singlePixel[16] = { 0U, 0U, 0U, 255U, 128, 0, 0, 255, 0, 128, 0, 255, 0, 0, 128, 255 };

};

#endif