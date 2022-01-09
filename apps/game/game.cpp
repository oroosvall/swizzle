
#include "game.hpp"

#include <swizzle/asset/TextureLoader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

Game::Game()
    : cam(glm::radians(45.0F), 1280, 720)
    , mController(cam)
    , guiLabel(nullptr)
{
}

Game::~Game()
{
}

void Game::userSetup()
{
    mWindow->setTitle("Space shooter");
    //mWindow->setSize(1280, 720);

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);
    mCmdBuffer = mGfxContext->createCommandBuffer(3);

    mPlayer.initResources(mGfxContext);

    sw::gfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {
        { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) }
    };
    attribs.mAttributes = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U },
        { 0U, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U }
    };
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;

    mShader = mSwapchain->createShader(attribs);
    mShader->load("shaders/simple.shader");

    mMaterial = mShader->createMaterial();

    mTexture = swizzle::asset::LoadTexture2D(mGfxContext, "texture/lightGray.png");
    mMaterial->setDescriptorTextureResource(0u, mTexture);

    cam.setPosition({ 45.0F, 0.0F, -15.0F });

    mUniformBuffer = mGfxContext->createBuffer(sw::gfx::BufferType::UniformBuffer);

    mUniformBuffer->setBufferData(&lampColor, sizeof(lampColor), 1);
    mMaterial->setDescriptorBufferResource(1u, mUniformBuffer, sizeof(lampColor));

    guiLabel = sw::gui::CreateLabel(mGfxContext);

    sw::gfx::ShaderAttributeList attribsText = {};
    attribsText.mBufferInput = {
        { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 2U) }
    };
    attribsText.mAttributes = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 3U }
    };
    attribsText.mEnableDepthTest = false;
    attribsText.mEnableBlending = true;

    mTextShader = mSwapchain->createShader(attribsText);
    mTextShader->load("shaders/text.shader");
    mTextMaterial = mShader->createMaterial();
    mTextMaterial->setDescriptorTextureResource(0u, guiLabel->getTexture());

    mBulletMesh = sw::asset::LoadMesh(mGfxContext, "meshes/game/bullet.swm", true);

    mEnemyShip = sw::asset::LoadMesh(mGfxContext, "meshes/game/TheTube.swm", true);

    sw::asset::LoadMesh(mGfxContext, "c:/gme/test.swm", true);

    mEnemyPosisions = {
        {0.0f, 5.0f, -70.0f,},
        {0.0f, 0.0f, -70.0f,},
        {0.0f, -5.0f, -70.0f, }
    };

    sw::gfx::ShaderAttributeList attribsSky = {};
    attribsSky.mBufferInput = {
        { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) }
    };
    attribsSky.mAttributes = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U },
        { 0U, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U }
    };;
    attribsSky.mEnableDepthTest = false;
    attribsSky.mEnableBlending = false;

    mSkyShader = mSwapchain->createShader(attribsSky);
    mSkyShader->load("shaders/sky.shader");

    mSkyMaterial = mSkyShader->createMaterial();

    mSkyTexture = sw::asset::LoadTextureCubeMap(mGfxContext,
        "texture/right.png", "texture/left.png", "texture/top.png", "texture/bottom.png", "texture/front.png", "texture/back.png");

    mSkyMaterial->setDescriptorTextureResource(0, mSkyTexture);

    mSkysphere = sw::asset::LoadMesh(mGfxContext, "meshes/inverted_sphere.obj", true);

}

void Game::userCleanup()
{
    delete guiLabel;

    mCmdBuffer.reset();
    mTextMaterial.reset();
    mTextShader.reset();

    mMaterial.reset();
    mUniformBuffer.reset();
    mTexture.reset();

    mShader.reset();
    mPlayer.cleanupResources();
    mBulletMesh.mVertexBuffer.reset();
    mBulletMesh.mIndexBuffer.reset();
}

SwBool Game::userUpdate(F32 dt)
{
    mWindow->pollEvents();
    mFpsCounter.tick(dt);
    mController.update(0.0F);
    mPlayer.update(dt);

    //printf("Loop\n");

    std::string title = "";

    title += "Frames: " + std::to_string(mSwapchain->getFrameCounter()) + "\n";
    title += "FPS: " + std::to_string(mFpsCounter.getFps());
    S32 frameTimeUs = S32((1000.0F / F32(mFpsCounter.getFps())) * 1000.0F);
    const SwChar* unit = "µ";
    if (frameTimeUs > 1000)
    {
        frameTimeUs /= 1000;
        unit = "m";
    }
    title += " (" + std::to_string(frameTimeUs) + " " + unit + "s)\n\n";

    //title += std::string(mGfxContext->getDeviceName(0u)) + "\n";
    title += "Num Bullets: " + std::to_string(mBulletPositions.size()) + "\n";
    //auto stats = mGfxContext->getStatistics();
    //title += "GFX: Gpu Memory usage: " + std::to_string(stats.mGpuMemoryUsage) + "\n";
    //title += "GFX: Cpu Memory usage: " + std::to_string(stats.mCpuMemoryUsage) + "\n";
    //title += "Staged objects: " + std::to_string(stats.mStagedObjects) + "\n";
    //title += "Num Textures: " + std::to_string(stats.mNumTextures) + "\n";
    //title += "Num Buffers: " + std::to_string(stats.mNumBuffers) + "\n";
    title += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    title += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    title += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";
    //title += "RemainingSize: " + std::to_string(guiLabel->getBuffer()->getRemainingSize()) + "\n";

    guiLabel->setText(title.c_str());

    if (mPlayer.shouldShoot())
    {
        mBulletPositions.emplace_back(mPlayer.getPosition());
    }

    for (auto& pos : mBulletPositions)
    {
        pos.z -= 10.0F * dt;
    }

    for (auto& pos : mEnemyPosisions)
    {
        pos.z += 5.0F * dt;
    }

    if (!mBulletPositions.empty())
    {
        mBulletPositions.erase(std::remove_if(mBulletPositions.begin(), mBulletPositions.end(), [](const auto& a) { return a.z < -100.0F; }), mBulletPositions.end());
    }

    U32 x, y;
    mWindow->getSize(x, y);
    cam.changeAspect((F32)x, (F32)y);

    sw::gfx::HSVA hsv = sw::gfx::RgbaToHsva(rgba);
    hsv.h += 20.0F * dt;
    rgba = sw::gfx::HsvaToRgba(hsv);

    //mSwapchain->setClearColor({ rgba.r, rgba.g, rgba.b, rgba.a });
    mSwapchain->setClearColor({ 0.01F, 0.01F, 0.01F });

    mSwapchain->prepare();
    mCmdBuffer->begin();

    mCmdBuffer->uploadTexture(mTexture);
    mCmdBuffer->uploadTexture(guiLabel->getTexture());
    mCmdBuffer->uploadTexture(mSkyTexture);

    mCmdBuffer->beginRenderPass(mSwapchain);

    struct tmp
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 eye;
    };

    tmp t = {};
    t.view = cam.getView();
    t.proj = cam.getProjection();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    mCmdBuffer->bindShader(mSkyShader);

    mCmdBuffer->bindMaterial(mSkyShader, mSkyMaterial);
    mCmdBuffer->setViewport(x, y);

    t.model = glm::translate(glm::mat4(1.0F), { cam.getPosition() });

    mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    mCmdBuffer->draw(mSkysphere.mVertexBuffer);

    t.model = mPlayer.getMatrix();
    mCmdBuffer->bindShader(mShader);

    mCmdBuffer->bindMaterial(mShader, mMaterial);
    mCmdBuffer->setViewport(x, y);

    mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    mCmdBuffer->drawIndexed(mPlayer.getVertexBuffer(), mPlayer.getIndexBuffer());
    //mCmdBuffer->draw(mPlayer.getBuffer());

    for (auto& pos : mBulletPositions)
    {
        t.model = glm::translate(glm::mat4(1.0F), pos);
        mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));
        //mCmdBuffer->draw(mBulletMesh.mVertexBuffer);
        mCmdBuffer->drawIndexed(mBulletMesh.mVertexBuffer, mBulletMesh.mIndexBuffer);
    }

    for (auto& pos : mEnemyPosisions)
    {
        t.model = glm::translate(glm::mat4(1.0F), pos);
        mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));
        mCmdBuffer->drawIndexed(mEnemyShip.mVertexBuffer, mEnemyShip.mIndexBuffer);
    }

    // Text stuff

    mCmdBuffer->bindShader(mTextShader);

    mCmdBuffer->setViewport(x, y);

#pragma pack(push, 1)
    struct Gui
    {
        glm::mat4 mat;
        glm::vec3 offest;
    };

#pragma pack(pop)
    Gui g;

    g.mat = glm::orthoRH_ZO(0.0F, (F32)x, (F32)y, 0.0F, 0.01F, 10.0F);
    g.offest = { 5.0F, 50.0F, 0.0F };

    mCmdBuffer->setShaderConstant(mTextShader, (U8*)&g, sizeof(g));
    mCmdBuffer->bindMaterial(mTextShader, mTextMaterial);

    mCmdBuffer->draw(guiLabel->getBuffer());

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();

    mGfxContext->submit(&mCmdBuffer, 1, mSwapchain);

    mSwapchain->present();

    return mWindow->isVisible();
}

