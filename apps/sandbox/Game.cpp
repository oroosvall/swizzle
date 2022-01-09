
#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include <swizzle/asset/TextureLoader.hpp>
#include <swizzle/asset/MeshLoader.hpp>

#include <cstring>
#include <array>

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
    guiLabel = sw::gui::CreateLabel(mGfxContext);
    guiLabel->setText("This is amazing!\n");

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.read(cfgFile.c_str());
    utils::Value v;
    v.setString(".");
    mGameCfg.setValue("AssetDir", v);

    //mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOn);
    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    mCmdBuffer = mGfxContext->createCommandBuffer(3);

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

    //mTexture = mGfxContext->createTexture(1024U, 1024U);

    //mTexture->setData(2, 2, 4, singlePixel);

    cam.setPosition({ 0.0F, 0.0F, 5.5F });

    mUniformBuffer = mGfxContext->createBuffer(sw::gfx::BufferType::UniformBuffer);

    mUniformBuffer->setBufferData(&lampColor, sizeof(lampColor), 1);
    mMaterial->setDescriptorBufferResource(1u, mUniformBuffer, sizeof(float) * 4);



    sw::gfx::ShaderAttributeList attribsSky = {};
    attribsSky.mBufferInput = {
        { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) }
    };
    attribsSky.mAttributes = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U },
        { 0U, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U }
    };
    attribsSky.mEnableDepthTest = false;
    attribsSky.mEnableBlending = false;

    mSkyShader = mSwapchain->createShader(attribsSky);
    mSkyShader->load("shaders/sky.shader");

    mSkyMaterial = mSkyShader->createMaterial();

    mSkyTexture = sw::asset::LoadTextureCubeMap(mGfxContext,
        "texture/right.png", "texture/left.png", "texture/top.png", "texture/bottom.png", "texture/front.png", "texture/back.png");

    mSkyMaterial->setDescriptorTextureResource(0, mSkyTexture);

    mSkysphere = sw::asset::LoadMesh(mGfxContext, "meshes/inverted_sphere.obj", true);

    // "meshes/main_city.obj"
    // "C:/tmp/cobra.obj"
    // "C:/tmp/dragon.obj"

    //mMesh = sw::asset::LoadMesh(mGfxContext, "meshes/main_city.obj", true);
    //mMesh = sw::asset::LoadMesh(mGfxContext, "C:/tmp/cobra.obj", true);
    //mMesh = sw::asset::LoadMesh(mGfxContext, "meshes/test.swm", true);
    mMeshAnimated = sw::asset::LoadMeshAnimated(mGfxContext, "c:/gme/test.swm", true);

    sw::gfx::ShaderAttributeList attribsAnim = {};
    attribsAnim.mBufferInput = {
        { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u + 4u + 4u) }
    };
    attribsAnim.mAttributes = {
        { 0u, sw::gfx::ShaderAttributeDataType::vec3f, 0u},
        { 0u, sw::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u },
        { 0u, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u },
        { 0u, sw::gfx::ShaderAttributeDataType::vec4u, sizeof(float) * 8u },
        { 0u, sw::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u }
    };
    attribsAnim.mEnableDepthTest = true;
    attribsAnim.mEnableBlending = false;

    mAnimationShader = mSwapchain->createShader(attribsAnim);
    mAnimationShader->load("shaders/animated.shader");
    mAnimationMaterial = mAnimationShader->createMaterial();
    mAnimationMaterial->setDescriptorTextureResource(0u, mSkyTexture);
    mAnimationMaterial->setDescriptorBufferResource(1u, mMeshAnimated.mBoneBuffer, ~0ull);
}

SwBool Game::userUpdate(F32 dt)
{
    OPTICK_EVENT("userUpdate");
    if (sw::input::WasPressedThisFrame(sw::input::Keys::Key0))
    {
        extraText++;
    }

    if (sw::input::WasPressedThisFrame(sw::input::Keys::KeyT))
    {
        testMode = !testMode;
    }

    if (sw::input::WasPressedThisFrame(sw::input::Keys::Key1))
    {
        selectedBone++;
    }
    if (sw::input::WasPressedThisFrame(sw::input::Keys::Key2))
    {
        selectedBone--;
    }

    mFpsCounter.tick(dt);

    auto stats = mGfxContext->getStatistics();

    std::string title;
    title.reserve(512);
    title = "Heljo world\n";

    title += "Frames: " + std::to_string(mSwapchain->getFrameCounter()) + "\n";
    title += "FPS: " + std::to_string(mFpsCounter.getFps()) + "\n\n";

    title += std::string(mGfxContext->getSelectedDeviceName()) + "\n";
    title += "GFX: Gpu Memory usage: " + std::to_string(stats.mGpuMemoryUsage / (1024 * 1024)) + "MB\n";
    title += "GFX: Cpu Memory usage: " + std::to_string(stats.mCpuMemoryUsage / (1024 * 1024)) + "MB\n";
    title += "Staged objects: " + std::to_string(stats.mStagedObjects) + "\n";
    title += "Num Textures: " + std::to_string(stats.mNumTextures) + "\n";
    title += "Num Buffers: " + std::to_string(stats.mNumBuffers) + "\n";
    title += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    title += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    title += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";
    title += "RemainingSize: " + std::to_string(guiLabel->getBuffer()->getRemainingSize()) + "\n";
    title += "TestMode (T): " + std::to_string(testMode) + "\n";

    for (int i = 0; i < extraText; i++)
    {
        title += "a";
    }

    //mWindow->setTitle(title.c_str());

    guiLabel->setText(title.c_str());

    mController.update(dt);

    sw::gfx::HSVA hsv = sw::gfx::RgbaToHsva(rgba);
    hsv.h += 20.0F * dt;
    rgba = sw::gfx::HsvaToRgba(hsv);

    sw::gfx::HSVA hsvLamp = sw::gfx::RgbaToHsva(lampColor);
    hsvLamp.h += 20.0F * dt;
    lampColor = sw::gfx::HsvaToRgba(hsvLamp);

    updateMainWindow();

    return mWindow->isVisible();
}

void Game::userCleanup()
{
    delete guiLabel;

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.write(cfgFile.c_str());

    mCmdBuffer.reset();

    mShader.reset();
    mMaterial.reset();

    mTexture.reset();

    mFrameBuffer.reset();

    mUniformBuffer.reset();

    mMesh.mVertexBuffer.reset();
    mMesh.mIndexBuffer.reset();

    mSkysphere.mVertexBuffer.reset();
    mSkysphere.mIndexBuffer.reset();

    mSkyShader.reset();

    mSkyTexture.reset();
    mSkyMaterial.reset();

    mTextShader.reset();
    mTextMaterial.reset();

    mMeshAnimated.mBoneBuffer.reset();
    mMeshAnimated.mIndexBuffer.reset();
    mMeshAnimated.mVertexBuffer.reset();

    mAnimationShader.reset();
    mAnimationMaterial.reset();
}

void Game::updateMainWindow()
{
    OPTICK_EVENT("Game::updateMainWindow");
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);

    void* mem = mUniformBuffer->mapMemory(sizeof(lampColor));
    memcpy(mem, &lampColor, sizeof(lampColor));
    mUniformBuffer->unmapMemory();

    mSwapchain->setClearColor({ rgba.r, rgba.g, rgba.b, rgba.a });

    struct tmp
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 eye;
        int sb;
    };

    tmp t = {};
    t.model = glm::mat4(1.0F);
    t.view = cam.getView();
    t.proj = cam.getProjection();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);
    t.sb = selectedBone;

    mSwapchain->prepare();
    mCmdBuffer->begin();

    mCmdBuffer->uploadTexture(mTexture);
    mCmdBuffer->uploadTexture(guiLabel->getTexture());
    mCmdBuffer->uploadTexture(mSkyTexture);

    mCmdBuffer->beginRenderPass(mSwapchain);

    mCmdBuffer->bindShader(mSkyShader);

    mCmdBuffer->bindMaterial(mSkyShader, mSkyMaterial);
    mCmdBuffer->setViewport(x, y);

    t.model = glm::translate(glm::mat4(1.0F), { cam.getPosition()});

    mCmdBuffer->setShaderConstant(mSkyShader, (U8*)&t, sizeof(t));

    mCmdBuffer->draw(mSkysphere.mVertexBuffer);


    mCmdBuffer->bindShader(mAnimationShader);

    mCmdBuffer->bindMaterial(mAnimationShader, mAnimationMaterial);
    mCmdBuffer->setViewport(x, y);


    t.model = glm::translate(glm::mat4(1.0F), { 0, 0, 0 });

    mCmdBuffer->setShaderConstant(mAnimationShader, (U8*)&t, sizeof(t));

    //mCmdBuffer->drawIndexed(mMesh.mVertexBuffer, mMesh.mIndexBuffer);
    mCmdBuffer->drawIndexed(mMeshAnimated.mVertexBuffer, mMeshAnimated.mIndexBuffer);

    if (testMode)
    {
        mCmdBuffer->bindShader(mShader);
        mCmdBuffer->bindMaterial(mShader, mMaterial);

        for (int i = 0; i < 25000; ++i)
        {
            t.model = glm::translate(glm::mat4(1.0F), {0, i, 0});
            mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));
            common::Resource<swizzle::gfx::Buffer> test = mGfxContext->createBuffer(swizzle::gfx::BufferType::Vertex);
            F32 data[] = {0.0, 0.0, 0.0};
            test->setBufferData(data, 3, 3);
            mCmdBuffer->draw(test);
        }
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
    mCmdBuffer->bindMaterial(mShader, mTextMaterial);

    mCmdBuffer->draw(guiLabel->getBuffer());

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();
    //mCmdBuffer->submit(mSwapchain);
    mGfxContext->submit(&mCmdBuffer, 1u, mSwapchain);
    mSwapchain->present();
    //mSwapchain->present();
}
