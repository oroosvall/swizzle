#ifndef GAME_HPP
#define GAME_HPP

#include <swizzle/Swizzle.hpp>

#include <utils/Config.hpp>
#include <utils/FpsCounter.hpp>

#include <swizzle/asset/MeshLoader.hpp>

#include "Camera.hpp"
#include "CameraController.hpp"

#include "player.hpp"

class Game : public sw::Application
{
public:

    Game();
    ~Game();

    virtual void userSetup() override;

    virtual SwBool userUpdate(F32 dt) override;
    virtual void userCleanup() override;

private:

    common::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    common::Resource<sw::gfx::Buffer> mUniformBuffer;

    sw::gfx::RGBA rgba = { 0.4F, 0.17F, 0.05F, 1.0F };
    sw::gfx::RGBA lampColor = { 0.8F, 0.2F, 0.2F, 1.0F };

    PerspectiveCamera cam;
    CameraController mController;

    Player mPlayer;

    common::Resource<sw::gfx::Shader> mShader;
    common::Resource<sw::gfx::Material> mMaterial;

    common::Resource<sw::gfx::Texture> mTexture;

    utils::FpsCounter mFpsCounter;

    std::vector<glm::vec3> mBulletPositions;
    sw::Mesh mBulletMesh;

    std::vector<glm::vec3> mEnemyPosisions;
    sw::Mesh mEnemyShip;

    sw::Mesh mSkysphere;

    common::Resource<sw::gfx::Shader> mSkyShader;

    common::Resource<sw::gfx::Texture> mSkyTexture;
    common::Resource<sw::gfx::Material> mSkyMaterial;

};

#endif