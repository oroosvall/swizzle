#ifndef GAME_HPP
#define GAME_HPP

#include <swizzle/Swizzle.hpp>
#include "Camera.hpp"
#include "CameraController.hpp"

#include <vector>

#include <utils/Config.hpp>
#include <utils/FpsCounter.hpp>

struct CameraBuffer
{
    glm::mat4 mCameraMatrix;
    glm::vec4 mCameraEye;
};

struct LampBuffer
{
    glm::vec4 mLightPos; // w contains raidus
    glm::vec4 mLightColor; // w contains intensity
    glm::vec4 mSpecColor;
};

struct MeshStuffs
{
    common::Resource<swizzle::gfx::Buffer> mMesh;
    common::Resource<swizzle::gfx::Buffer> mInstance;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;
};

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

    void updateSkyTime();

    void loadSky();
    void loadMesh();

    // Shared ptr sent as reference here is intended,
    void drawSky(common::Resource<sw::gfx::CommandBuffer>& cmd);
    // Shared ptr sent as reference here is intended,
    void drawBuilding(common::Resource<sw::gfx::CommandBuffer>& cmd);

    common::Resource<sw::gfx::CommandBuffer> mCmdBuffer;

    common::Resource<sw::gfx::FrameBuffer> mGbuffer;
    float mFogDist = 30.0f;
    SwBool mEnableFog = true;

    bool mInputLocked = false;

    S32 mSelectedOption = 0;
    std::vector<sw::gfx::VSyncTypes> mVSyncOptions;

    PerspectiveCamera cam;
    CameraController mController;

    utils::FpsCounter mFpsCounter;

    common::Resource<sw::gfx::Shader> mFsq;
    common::Resource<sw::gfx::Material> mFsqMat;

    common::Resource<swizzle::gfx::Buffer> mCameraBuffer;
    common::Resource<swizzle::gfx::Buffer> mLampBuffer;

    MeshStuffs mBuilding;
    MeshStuffs mSky;

    F32 mTime;
    F32 mSkytime;
    F32 mSkyCycleTime;
    bool mPauseTime = false;
    glm::vec3 mSunMoonDir;

    //common::Resource<swizzle::gfx::Buffer> mMesh;
    //common::Resource<swizzle::gfx::Buffer> mInstance;
    //common::Resource<swizzle::gfx::Material> mMaterial;
    //common::Resource<swizzle::gfx::Shader> mShader;
};

#endif