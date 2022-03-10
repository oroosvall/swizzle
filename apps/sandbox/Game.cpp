
#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include <swizzle/asset/TextureLoader.hpp>
#include <swizzle/asset/MeshLoader.hpp>

#include <cstring>
#include <array>

#include <utils/StringUtils.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_swizzle.hpp>

ImVec4 col = { 0.5, 0.5, 0.5, 1.0 };

Game::Game()
    : cam(glm::radians(45.0F), 1280, 720)
    , mController(cam)
{
}

Game::~Game()
{
}

void Game::userSetup()
{
    //mWindow->setCursorVisible(false);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSwizzle_Init(mGfxContext, mWindow);

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.read(cfgFile.c_str());
    utils::Value v;
    v.setString(".");
    mGameCfg.setValue("AssetDir", v);

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOn);
    //mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

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

    sw::gfx::ShaderAttributeList attribFsq = {};
    attribFsq.mEnableBlending = true;

    mFsq = mSwapchain->createShader(attribFsq);
    mFsq->load("shaders/fsq.shader");

    mFsqMat = mFsq->createMaterial();
    ImGui_ImplSwizzle_SetMaterial(mFsqMat);

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
    //mMeshAnimated = sw::asset::LoadMeshAnimated(mGfxContext, "c:/gme/test.swm", true);
    mMeshAnimated = sw::asset::LoadMeshAnimated(mGfxContext, "meshes/test.swm", true);

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

    if (sw::input::WasKeyPressedThisFrame(sw::input::Keys::KeyT))
    {
        testMode = !testMode;
    }

    if (sw::input::WasKeyPressedThisFrame(sw::input::Keys::KeyR))
    {
        mSwapchain->resize();
    }

    mFpsCounter.tick(dt);

    // auto stats = mGfxContext->getStatistics();

    std::string title;
    title.reserve(1024);
    title = "Heljo world\n";

    title += "Frames: " + std::to_string(mSwapchain->getFrameCounter()) + "\n";
    title += "FPS: " + std::to_string(mFpsCounter.getFps()) + "\n\n";

    title += std::string(mGfxContext->getSelectedDeviceName()) + "\n";

    auto iter = mGfxContext->getStatisticsIterator();

    do
    {
        OPTICK_EVENT("Parse Stats");
        if (iter->getType() == sw::gfx::GfxStatsType::MemoryStats)
        {
            sw::gfx::MemoryStatistics* memStat = (sw::gfx::MemoryStatistics*)iter->getStatisticsData();

            title += "Memory Heap: " + std::string(memStat->mName) + "\n";
            title += "  Mem: " + utils::toMemoryString(memStat->mUsed) + "/" + utils::toMemoryString(memStat->mSize);
            title += "; Allocs: " + std::to_string(memStat->mNumAllocations) + "p, " + std::to_string(memStat->mNumVirtualAllocations) + "v\n";
        }
        else if (iter->getType() == sw::gfx::GfxStatsType::DeviceStats)
        {
            sw::gfx::DeviceStatistics* devStats = (sw::gfx::DeviceStatistics*)iter->getStatisticsData();
            title += "Device\n";
            title += "  Num Staged Objects: " + std::to_string(devStats->mNumStagedObjects) + "\n";
            title += "  Num Textures: " + std::to_string(devStats->mNumTextures) + "\n";
            title += "  Num Buffers: " + std::to_string(devStats->mNumBuffers) + "\n";
            title += "  Pipelines: " + std::to_string(devStats->mNumPipelines) + "\n";
        }

    } while (iter->next());

    /*title += "Staged objects: " + std::to_string(stats.mStagedObjects) + "\n";
    title += "Num Textures: " + std::to_string(stats.mNumTextures) + "\n";
    title += "Num Buffers: " + std::to_string(stats.mNumBuffers) + "\n";*/
    title += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    title += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    title += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";
    title += "TestMode (T): " + std::to_string(testMode) + "\n";

    //mWindow->setTitle(title.c_str());

    //guiLabel->setText(title.c_str());
    //mLabel->setText(title.c_str());

    ImGui_ImplSwizzle_NewFrame(mWindow);
    ImGui::NewFrame();

    ImGui::Begin("Blah");
    {
        OPTICK_EVENT("ImGui::Text");
        ImGui::Text("%s", title.c_str());
    }

    ImGui::ColorEdit4("Color", (float*)&col);

    ImGui::End();

    ImGui::EndFrame();

    //mController.update(dt);

    sw::gfx::HSVA hsv = sw::gfx::RgbaToHsva(rgba);
    hsv.h += 20.0F * dt;
    rgba = sw::gfx::HsvaToRgba(hsv);

    sw::gfx::HSVA hsvLamp = sw::gfx::RgbaToHsva(lampColor);
    hsvLamp.h += 20.0F * dt;
    lampColor = sw::gfx::HsvaToRgba(hsvLamp);

    updateMainWindow(dt);

    return mWindow->isVisible();
}

void Game::userCleanup()
{
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

    mMeshAnimated.mBoneBuffer.reset();
    mMeshAnimated.mIndexBuffer.reset();
    mMeshAnimated.mVertexBuffer.reset();

    mAnimationShader.reset();
    mAnimationMaterial.reset();
}

void Game::updateMainWindow(F32 dt)
{
    UNUSED_ARG(dt);
    OPTICK_EVENT("Game::updateMainWindow");
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);

    void* mem = mUniformBuffer->mapMemory(sizeof(lampColor));
    memcpy(mem, &lampColor, sizeof(lampColor));
    mUniformBuffer->unmapMemory();

    //mSwapchain->setClearColor({ rgba.r, rgba.g, rgba.b, rgba.a });
    mSwapchain->setClearColor({ 0,0,0,1 });

    struct tmp
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 eye;
    };

    tmp t = {};
    t.model = glm::mat4(1.0F);
    t.view = cam.getView();
    t.proj = cam.getProjection();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    mSwapchain->prepare();
    mCmdBuffer->begin();

    mCmdBuffer->uploadTexture(mTexture);
    mCmdBuffer->uploadTexture(mSkyTexture);

    ImGui_ImplSwizzle_UploadFontTexture(mCmdBuffer);

    ImGui_ImplSwizzle_BeginDraw(mCmdBuffer);

    ImGui::Render();
    ImGui_ImplSwizzle_RenderDrawData(ImGui::GetDrawData(), mCmdBuffer);
    ImGui_ImplSwizzle_EndDraw(mCmdBuffer);

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

    mCmdBuffer->bindShader(mFsq);
    mCmdBuffer->bindMaterial(mFsq, mFsqMat);
    mCmdBuffer->drawNoBind(3u, 0u);

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();
    //mCmdBuffer->submit(mSwapchain);
    mGfxContext->submit(&mCmdBuffer, 1u, mSwapchain);
    mSwapchain->present();
    //mSwapchain->present();
}
