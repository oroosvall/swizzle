#ifndef GAME_HPP
#define GAME_HPP

#include <swizzle/Swizzle.hpp>
#include "Camera.hpp"
#include "CameraController.hpp"

#include "Model.hpp"


class Game
{
public:

    Game();
    ~Game();

    bool update(float dt);
    void cleanup();

private:

    sw::core::Resource<sw::core::Window> mWindow;
    sw::core::Resource<sw::gfx::Swapchain> mSwapchain;
    sw::core::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    sw::core::Resource<sw::gfx::Shader> mShader;
    sw::core::Resource<sw::gfx::Buffer> mBuffer;

    sw::core::Resource<sw::gfx::Texture> mTexture;

    sw::core::Resource<sw::gfx::FrameBuffer> mFrameBuffer;

    sw::gfx::RGBA rgba = { 0.4F, 0.17F, 0.05F, 1.0F };
    PerspectiveCamera cam;
    CameraController mController;

    Model* mMesh;

};

#endif