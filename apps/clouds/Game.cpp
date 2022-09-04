
#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include <swizzle/asset/TextureLoader.hpp>

#include <utils/StringUtils.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_swizzle.hpp>

#include <swizzle/asset2/Assets.hpp>

Game::Game()
    : cam(glm::radians(45.0F), 1280, 720)
    , mController(cam)
    , mTime(0.0f)
{
}

Game::~Game()
{
}

void Game::userSetup()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSwizzle_Init(mGfxContext, mWindow);


    if (mSwapchain->isVsyncModeSupported(sw::gfx::VSyncTypes::vSyncOn)) mVSyncOptions.push_back(sw::gfx::VSyncTypes::vSyncOn);
    if (mSwapchain->isVsyncModeSupported(sw::gfx::VSyncTypes::vSyncAdaptive)) mVSyncOptions.push_back(sw::gfx::VSyncTypes::vSyncAdaptive);
    if (mSwapchain->isVsyncModeSupported(sw::gfx::VSyncTypes::vSyncOff)) mVSyncOptions.push_back(sw::gfx::VSyncTypes::vSyncOff);

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOn);
    //mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    mCmdBuffer = mGfxContext->createCommandBuffer(2);

    cam.setPosition({ 0.0F, 0.0F, 5.5F });

    sw::gfx::ShaderAttributeList attribFsq = {};
    attribFsq.mDescriptors = {
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
    };
    attribFsq.mPushConstantSize = sizeof(glm::vec4) + 4u;
    attribFsq.mEnableBlending = false;

    mFsq = mGfxContext->createShader(mSwapchain, attribFsq);
    mFsq->load("clouds-app/shaders/fsq.shader");

    mFsqMat = mGfxContext->createMaterial(mFsq);
    ImGui_ImplSwizzle_SetMaterial(mFsqMat);

    mCameraBuffer = mGfxContext->createBuffer(sw::gfx::BufferType::UniformBuffer);
    mLampBuffer = mGfxContext->createBuffer(sw::gfx::BufferType::UniformBuffer);

    LampBuffer lampBuf {};
    lampBuf.mLightPos = glm::vec4(0.0f, 10.0f, 1.0f, 100.0f); // w is radius
    lampBuf.mLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // w is intensity
    lampBuf.mSpecColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // w is unused

    mLampBuffer->setBufferData(&lampBuf, sizeof(LampBuffer), sizeof(LampBuffer));

    sw::gfx::FrameBufferCreateInfo gbufferCreateInfo{};
    gbufferCreateInfo.mDepthType = sw::gfx::FrameBufferDepthType::DepthOnly;
    gbufferCreateInfo.mSwapCount = 2u;
    gbufferCreateInfo.mNumColAttach = 3u;
    gbufferCreateInfo.mHeight = 1080;
    gbufferCreateInfo.mWidth = 1920;
    gbufferCreateInfo.mAttachmentType = sw::gfx::FrameBufferAttachmentType::F32;

    mGbuffer = mGfxContext->createFramebuffer(gbufferCreateInfo);

    mFsqMat->setDescriptorTextureResource(0u, mGbuffer->getColorAttachment(0u));
    mFsqMat->setDescriptorTextureResource(1u, mGbuffer->getColorAttachment(1u));
    mFsqMat->setDescriptorTextureResource(2u, mGbuffer->getColorAttachment(2u));

    mSunMoonDir = glm::vec3(1.0, 0.0, 0.0);
    mSkyCycleTime = 120.0f;

    loadSky();
    loadMesh();
}

void Game::updateSkyTime()
{
    F32 skyTime = glm::mod(mTime, mSkyCycleTime);
    if (skyTime > mSkyCycleTime)
    {
        skyTime -= mSkyCycleTime;
    }

    mSkytime = skyTime / mSkyCycleTime;

    const float r = 100.0F;
    const float PI = glm::pi<float>();

    float s = (glm::abs(glm::sin(PI * mSkytime * 2.0F + (PI * 0.5F))) * 1.6F) - 0.6F;
    float c = glm::cos(PI * mSkytime * 2.0F + (PI * 0.5F));

    if (mSkytime >= 0.25F && mSkytime <= 0.75F)
    {
        c = -c;
    }
    if (s > 0.0F)
    {
        s *= 0.2F;
    }

    mSunMoonDir = glm::vec3(-r * 0.4F, r * s, r * c);

    LampBuffer lampBuf{};
    lampBuf.mLightPos = glm::vec4(mSunMoonDir, 1000.0f); // w is radius
    lampBuf.mLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // w is intensity
    lampBuf.mSpecColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // w is unused

    mLampBuffer->setBufferData(&lampBuf, sizeof(LampBuffer), sizeof(LampBuffer));

    mSunMoonDir = normalize(glm::vec3(0) - mSunMoonDir);
}

void Game::loadSky()
{
    swizzle::gfx::ShaderAttributeList attribsSky = {};
    attribsSky.mBufferInput = {
        { swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, (sizeof(float) * 3u) },
        { swizzle::gfx::ShaderBufferInputRate::InputRate_Instance, (sizeof(float) * 16u) }
    };
    attribsSky.mAttributes = {
        { 0U, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 0u },
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u },
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u },
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u },
    };
    attribsSky.mDescriptors = {
         {swizzle::gfx::DescriptorType::UniformBuffer, swizzle::gfx::Count(1u), {swizzle::gfx::StageType::vertexStage, swizzle::gfx::StageType::fragmentStage}},
         {swizzle::gfx::DescriptorType::UniformBuffer, swizzle::gfx::Count(1u), {swizzle::gfx::StageType::fragmentStage}},
    };
    attribsSky.mPushConstantSize = sizeof(glm::vec4) * 2u + 4u;
    attribsSky.mEnableDepthTest = false;
    attribsSky.mEnableBlending = false;

    mSky.mShader = mGfxContext->createShader(mGbuffer, attribsSky);

    if (!mSky.mShader->load("clouds-app/shaders/sky.shader"))
    {
        LOG_ERROR("Failed to load shader");
    }

    mSky.mMaterial = mGfxContext->createMaterial(mSky.mShader);

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u}
    };

    auto mesh = swizzle::asset2::LoadMesh("clouds-app/meshes/SkySphere_c.swm", ldi);

    mSky.mMesh = mGfxContext->createBuffer(swizzle::gfx::BufferType::Vertex);
    mSky.mMesh->setBufferData((U8*)mesh->getVertexDataPtr(), mesh->getVertexDataSize(), (sizeof(float) * (3u)));

    mSky.mInstance = mGfxContext->createBuffer(swizzle::gfx::BufferType::Vertex);
    glm::mat4 mdlMat = glm::mat4(1.0f);
    mSky.mInstance->setBufferData(&mdlMat, sizeof(glm::mat4), sizeof(glm::mat4));
}

void Game::loadMesh()
{
    swizzle::gfx::ShaderAttributeList attribsSky = {};
    attribsSky.mBufferInput = {
        { swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, (sizeof(float) * 10u) },
        { swizzle::gfx::ShaderBufferInputRate::InputRate_Instance, (sizeof(float) * 16u) }
    };
    attribsSky.mAttributes = {
        { 0U, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        { 0U, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u },
        { 0U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 6u },
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 0u },
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u },
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u },
        { 1U, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u },
    };
    attribsSky.mDescriptors = {
         {swizzle::gfx::DescriptorType::UniformBuffer, swizzle::gfx::Count(1u), {swizzle::gfx::StageType::vertexStage}},
         {swizzle::gfx::DescriptorType::UniformBuffer, swizzle::gfx::Count(1u), {swizzle::gfx::StageType::fragmentStage}},
    };
    attribsSky.mPushConstantSize = 0u;
    attribsSky.mEnableDepthTest = true;
    attribsSky.mEnableBlending = false;

    mBuilding.mShader = mGfxContext->createShader(mGbuffer, attribsSky);

    if (!mBuilding.mShader->load("clouds-app/shaders/vertexColorGbuf.shader"))
    {
        LOG_ERROR("Failed to load shader");
    }

    mBuilding.mMaterial = mGfxContext->createMaterial(mBuilding.mShader);

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector,   sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::VertexColor,  sizeof(float) * 6u},
    };

    auto mesh = swizzle::asset2::LoadMesh("clouds-app/meshes/Building.swm", ldi);

    mBuilding.mMesh = mGfxContext->createBuffer(swizzle::gfx::BufferType::Vertex);
    mBuilding.mMesh->setBufferData((U8*)mesh->getVertexDataPtr(), mesh->getVertexDataSize(), (sizeof(float) * (10u)));

    mBuilding.mInstance = mGfxContext->createBuffer(swizzle::gfx::BufferType::Vertex);
    glm::mat4 mdlMat = glm::mat4(1.0f);
    mBuilding.mInstance->setBufferData(&mdlMat, sizeof(glm::mat4), sizeof(glm::mat4));

}

void Game::drawSky(common::Resource<sw::gfx::CommandBuffer>& cmd)
{
    float x, y;

    cam.getCameraSize(x, y);

    mSky.mMaterial->setDescriptorBufferResource(0u, mCameraBuffer, sizeof(CameraBuffer));
    mSky.mMaterial->setDescriptorBufferResource(1u, mLampBuffer, sizeof(LampBuffer));

    cmd->bindShader(mSky.mShader);

    cmd->bindMaterial(mSky.mShader, mSky.mMaterial);
    cmd->setViewport(U32(x), U32(y));

    auto& viewMat = cam.getView();

    struct SkyPush
    {
        glm::vec4 eyeDir;
        glm::vec4 sunMoonDir;
        float globalTime;
    } skyPush {};

    skyPush.eyeDir = glm::vec4(viewMat[1][3], viewMat[2][3], viewMat[3][3], mSkytime);
    skyPush.sunMoonDir = glm::vec4(mSunMoonDir, 0.0);
    skyPush.globalTime = mTime;

    cmd->setShaderConstant(mSky.mShader, (U8*)&skyPush, sizeof(skyPush));

    cmd->drawInstanced(mSky.mMesh, mSky.mInstance);
}

void Game::drawBuilding(common::Resource<sw::gfx::CommandBuffer>& cmd)
{
    float x, y;

    cam.getCameraSize(x, y);

    mBuilding.mMaterial->setDescriptorBufferResource(0u, mCameraBuffer, sizeof(CameraBuffer));
    mBuilding.mMaterial->setDescriptorBufferResource(1u, mLampBuffer, sizeof(LampBuffer));

    cmd->bindShader(mBuilding.mShader);

    cmd->bindMaterial(mBuilding.mShader, mBuilding.mMaterial);
    cmd->setViewport(U32(x), U32(y));

    //cmd->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    cmd->drawInstanced(mBuilding.mMesh, mBuilding.mInstance);
}


SwBool Game::userUpdate(F32 dt)
{
    if (!mPauseTime)
        mTime += dt;

    OPTICK_EVENT("userUpdate");

    mFpsCounter.tick(dt);

    std::string title;
    title.reserve(1024);
    //title = "Heljo world\n";

    title += "Frames: " + std::to_string(mSwapchain->getFrameCounter()) + "\n";
    title += "FPS: " + std::to_string(mFpsCounter.getFps()) + "\n\n";

    title += std::string(mGfxContext->getSelectedDeviceName()) + "\n";

    auto iter = mGfxContext->getStatisticsIterator();

    /*if (sw::input::WasKeyPressedThisFrame(sw::input::Keys::KeyF7))
    {
        mInputLocked = !mInputLocked;
    }*/

    mInputLocked = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;

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
        else if (iter->getType() == sw::gfx::GfxStatsType::InstanceStats)
        {
            sw::gfx::InstanceStatistics* instStats = (sw::gfx::InstanceStatistics*)iter->getStatisticsData();
            title += "Instance\n";
            title += "  Alloc Count " + std::to_string(instStats->mAllocCount) + "\n";
            title += "  Internal Alloc Count " + std::to_string(instStats->mInternalAllocCount) + "\n";
        }

    } while (iter->next());


    title += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    //title += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    //title += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";

    title += "Input locked: " + std::to_string(mInputLocked);

    ImGui_ImplSwizzle_NewFrame(mWindow);
    ImGui::NewFrame();

    ImGui::Begin("Statistics");
    {
        OPTICK_EVENT("ImGui::Text");
        ImGui::Text("%s", title.c_str());
    }

    ImGui::End();

    auto cmb = [](void* data, S32 idx, const char** outText) -> bool {
        sw::gfx::VSyncTypes* pData = (sw::gfx::VSyncTypes*)data;

        switch (pData[idx])
        {
        case sw::gfx::VSyncTypes::vSyncAdaptive:
            *outText = "Vsync Adaptive";
            return true;
        case sw::gfx::VSyncTypes::vSyncOff:
            *outText = "Vsync Off";
            return true;
        case sw::gfx::VSyncTypes::vSyncOn:
            *outText = "Vsync On";
            return true;
        default:
            break;
        }
        return false;
    };

    ImGui::Begin("Options");
    ImGui::Text("Vsync: ");
    ImGui::SameLine();
    if (ImGui::Combo("##0", &mSelectedOption, cmb, (void*)mVSyncOptions.data(), (S32)mVSyncOptions.size(), -1))
    {
        mSwapchain->setVsync(mVSyncOptions[mSelectedOption]);
    }
    ImGui::Text("Enable fog: ");
    ImGui::SameLine();
    ImGui::Checkbox("##1", &mEnableFog);
    ImGui::Text("FogDensity: ");
    ImGui::SameLine();
    ImGui::SliderFloat("##2", &mFogDist, 0.01f, 150.0f);


    ImGui::Text("Pause time: ");
    ImGui::SameLine();
    ImGui::Checkbox("##4", &mPauseTime);
    ImGui::Text("Time cycle in seconds: ");
    ImGui::SameLine();
    if (ImGui::SliderFloat("##3", &mSkyCycleTime, 24.0f, 120.0f))
    {
        mTime = 0.0f;
    }

    if (ImGui::Button("Reload shader"))
    {
        if (!mBuilding.mShader->load("clouds-app/shaders/vertexColorGbuf.shader"))
        {
            LOG_ERROR("Failed to load color shader");
        }
        if (!mSky.mShader->load("clouds-app/shaders/sky.shader"))
        {
            LOG_ERROR("Failed to load sky shader");
        }
        if (!mFsq->load("clouds-app/shaders/fsq.shader"))
        {
            LOG_ERROR("Failed to fullscreen blit shader");
        }
    }

    ImGui::End();

    ImGui::EndFrame();

    updateSkyTime();

    updateMainWindow(dt);

    return mWindow->isVisible();
}

void Game::userCleanup()
{
    mCmdBuffer.reset();
    mGbuffer.reset();

    mFsq.reset();
    mFsqMat.reset();
}

void Game::updateMainWindow(F32 dt)
{
    UNUSED_ARG(dt);
    OPTICK_EVENT("Game::updateMainWindow");
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);
    if (!mInputLocked)
    {
        mController.update(dt);
    }

    CameraBuffer camBuf {};

    camBuf.mCameraMatrix = cam.getProjection() * cam.getView();
    camBuf.mCameraEye = glm::vec4(cam.getPosition(), 1.0f);
    mCameraBuffer->setBufferData(&camBuf, sizeof(CameraBuffer), sizeof(CameraBuffer));

    glm::mat4 mat = glm::translate(glm::mat4(1.0f), cam.getPosition());
    mSky.mInstance->setBufferData(&mat, sizeof(mat), sizeof(mat));

    mSwapchain->setClearColor({ 0,0,0,1 });

    mSwapchain->prepare();
    mCmdBuffer->begin();

    imGuiRender();

    F32 inf = std::numeric_limits<F32>::infinity();

    mGbuffer->setColorAttachmentClearColor(0u, { 0.0, 0.0, 0.0, 0.0 });
    mGbuffer->setColorAttachmentClearColor(1u, { 0.0, 0.0, 0.0, 0.0 });
    mGbuffer->setColorAttachmentClearColor(2u, { inf, inf, inf, 0.0 });
    mGbuffer->setDepthAttachmentClearValue(1.0f, 0);
    mCmdBuffer->beginRenderPass(mGbuffer);

    // render to gBuffer
    drawSky(mCmdBuffer);
    drawBuilding(mCmdBuffer);

    mCmdBuffer->endRenderPass();
    mCmdBuffer->beginRenderPass(mSwapchain);

    // Render to screen blitting all stuff together
    struct blah
    {
        glm::vec4 eye;
        S32 flags;
    } b {};

    b.eye = glm::vec4(cam.getPosition(), mFogDist);
    b.flags = mEnableFog;

    mCmdBuffer->bindShader(mFsq);
    mCmdBuffer->bindMaterial(mFsq, mFsqMat);
    mCmdBuffer->setShaderConstant(mFsq, (U8*)&b, sizeof(b));
    mCmdBuffer->drawNoBind(3u, 0u);

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();

    mGfxContext->submit(&mCmdBuffer, 1u, mSwapchain);
    mSwapchain->present();
}

void Game::imGuiRender()
{
    ImGui_ImplSwizzle_UploadFontTexture(mCmdBuffer);

    ImGui_ImplSwizzle_BeginDraw(mCmdBuffer);

    ImGui::Render();
    ImGui_ImplSwizzle_RenderDrawData(ImGui::GetDrawData(), mCmdBuffer);
    ImGui_ImplSwizzle_EndDraw(mCmdBuffer);
}
