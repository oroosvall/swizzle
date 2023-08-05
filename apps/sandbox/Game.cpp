
#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include <utils/StringUtils.hpp>

#include <imgui/imgui.h>
#include <ImGuiSwzzle.hpp>

#include <scene/Scene.hpp>


Game::Game(const SwChar* appName)
    : Application(appName)
    , cam(glm::radians(45.0F), 1280, 720)
    , mController(cam)
{
}

Game::~Game() {}

void Game::userSetup()
{
    // mWindow->setCursorVisible(false);

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.read(cfgFile.c_str());

    swizzle::gfx::FrameBufferCreateInfo fboInfo{};
    fboInfo.mDepthType = swizzle::gfx::FrameBufferDepthType::DepthNone;
    fboInfo.mSwapCount = 3u;
    fboInfo.mColorAttachFormats = { swizzle::gfx::FrameBufferAttachmentType::Default };
    mWindow->getSize(fboInfo.mWidth, fboInfo.mHeight);
    mLastWidth = fboInfo.mWidth;
    mLastHeight = fboInfo.mHeight;

    mImGuiFbo = mGfxDevice->createFramebuffer(fboInfo);

    mImGuiRenderTarget = common::CreateRef<ImGuiSwizzleRenderTarget>(mImGuiFbo);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplSwizzle_Init(mWindow, mGfxDevice, mImGuiRenderTarget);

    mAssetManager = common::CreateRef<AssetManager>(mGfxDevice);
    mCompositor = common::CreateRef<Compositor>(mGfxDevice, mSwapchain);
    mScene = common::CreateRef<Scene>(mGfxDevice, mAssetManager, mCompositor);
    mScene->loadScene("scenes/test.scene");
    mScene->loadSky();
    mScene->loadAnimMesh();

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOn);
    // mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    mCmdBuffer = mGfxDevice->createCommandBuffer(2);
    mGfxDevice->enablePipelineStatistics(true);

    cam.setPosition({0.0F, 0.0F, 5.5F});

    sw::gfx::ShaderAttributeList attribFsq = {};
    attribFsq.mDescriptors = {
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
    };
    attribFsq.mPushConstantSize = 0u;
    attribFsq.mEnableBlending = true;
    attribFsq.mPrimitiveType = sw::gfx::PrimitiveType::triangle;

    mFsq = mGfxDevice->createShader(mSwapchain, sw::gfx::ShaderType::ShaderType_Graphics, attribFsq);
    mFsq->load("shaders/fsq.shader");

    mFsqMat = mGfxDevice->createMaterial(mFsq, sw::gfx::SamplerMode::SamplerModeClamp);
    mFsqMat->setDescriptorTextureResource(0u, mImGuiRenderTarget->getTexture(), false);
}

SwBool Game::userUpdate(F32 dt)
{
    SWIZZLE_PROFILE_EVENT("userUpdate");

    mFpsCounter.tick(dt);

    std::string title;
    title.reserve(1024);
    title = "Heljo world\n";

    title += "Frames: " + std::to_string(mSwapchain->getFrameCounter()) + "\n";
    title += "FPS: " + std::to_string(mFpsCounter.getFps()) + "\n\n";

    title += std::string(mGfxDevice->getDeviceName()) + "\n";

    auto iter = mGfxDevice->getStatisticsIterator();

    do
    {
        SWIZZLE_PROFILE_EVENT("Parse Stats");
        if (iter->getType() == sw::gfx::GfxStatsType::MemoryStats)
        {
            sw::gfx::MemoryStatistics* memStat = (sw::gfx::MemoryStatistics*)iter->getStatisticsData();
            if (memStat)
            {
                title += "Memory Heap: " + std::string(memStat->mName) + "\n";
                title += "  Mem: " + utils::toMemoryString(memStat->mUsed) + "/" + utils::toMemoryString(memStat->mSize);
                title += "; Allocs: " + std::to_string(memStat->mNumAllocations) + "p, " +
                            std::to_string(memStat->mNumVirtualAllocations) + "v\n";
            }
        }
        else if (iter->getType() == sw::gfx::GfxStatsType::DeviceStats)
        {
            sw::gfx::DeviceStatistics* devStats = (sw::gfx::DeviceStatistics*)iter->getStatisticsData();
            if (devStats)
            {
                title += "Device\n";
                title += "  Num Staged Objects: " + std::to_string(devStats->mNumStagedObjects) + "\n";
                title += "  Num Textures: " + std::to_string(devStats->mNumTextures) + "\n";
                title += "  Num Buffers: " + std::to_string(devStats->mNumBuffers) + "\n";
                title += "  Pipelines: " + std::to_string(devStats->mNumPipelines) + "\n";
            }
        }
        else if (iter->getType() == sw::gfx::GfxStatsType::InstanceStats)
        {
            sw::gfx::InstanceStatistics* instStats = (sw::gfx::InstanceStatistics*)iter->getStatisticsData();
            if (instStats)
            {
                title += "Instance\n";
                title += "  Alloc Count " + std::to_string(instStats->mAllocCount) + "\n";
                title += "  Internal Alloc Count " + std::to_string(instStats->mInternalAllocCount) + "\n";
            }
        }
        else if (iter->getType() == sw::gfx::GfxStatsType::GfxPipelineStats)
        {
            sw::gfx::GfxPipelineStatistics* gfxStats = (sw::gfx::GfxPipelineStatistics*)iter->getStatisticsData();
            if (gfxStats)
            {
                title += "Graphics Pipeline statistics\n";
                title += "  Input Assemby Vertices " + std::to_string(gfxStats->mInputAssemblyVertices) + "\n";
                title += "  Input Assemby Primitives " + std::to_string(gfxStats->mInputAssemblyPrimitives) + "\n";
                title += "  Vertex Shader Invocations " + std::to_string(gfxStats->mVertexShaderInvocations) + "\n";
                title += "  Clipping Invocations " + std::to_string(gfxStats->mClippingInvocations) + "\n";
                title += "  Clipping Primitives " + std::to_string(gfxStats->mClippingInvocations) + "\n";
                title += "  Fragment Shader Invocations " + std::to_string(gfxStats->mFragmentShaderInvocations) + "\n";
                title += "  Tesselation Control Shader Patches " +
                        std::to_string(gfxStats->mTesselationControlShaderPatches) + "\n";
                title += "  Tesselation Evaluation Shader Invocations " +
                        std::to_string(gfxStats->mTesselationEvaluationShaderInvocations) + "\n";
                title += "  Compute Shader Invocations " + std::to_string(gfxStats->mComputeShaderInvocations) + "\n";
            }
        }

    } while (iter->next());

    title += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    title += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    title += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";

    ImGui_ImplSwizzle_NewFrame(dt);
    ImGui::NewFrame();

    ImGui::Begin("Blah");
    {
        SWIZZLE_PROFILE_EVENT("ImGui::Text");
        ImGui::Text("%s", title.c_str());
    }

    ImGui::End();

    ImGui::EndFrame();

    updateMainWindow(dt);

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    return mWindow->isVisible();
}

void Game::userCleanup()
{
    ImGui_ImplSwizzle_Shutdown();
    ImGui::DestroyContext();

    mImGuiFbo.reset();
    mImGuiRenderTarget.reset();

    mFsq.reset();
    mFsqMat.reset();

    mScene.reset();
    mAssetManager.reset();
    mCompositor.reset();

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.write(cfgFile.c_str());

    mCmdBuffer.reset();
}

void Game::updateMainWindow(F32 dt)
{
    UNUSED_ARG(dt);
    SWIZZLE_PROFILE_EVENT("Game::updateMainWindow");
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);
    if (!(ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse))
    {
        mController.update(dt);
    }

    if ((mLastWidth != x || mLastHeight != y) && mWindow->isVisible())
    {
        mImGuiFbo->resize(x, y);
        mLastWidth = x;
        mLastHeight = y;
        mFsqMat->setDescriptorTextureResource(0u, mImGuiRenderTarget->getTexture(), false);
    }

    mSwapchain->setClearColor({0, 0, 0, 1});

    mSwapchain->prepare();

    auto trans = mCmdBuffer->begin();

    mScene->update(dt, trans);

    imGuiRender(trans);

    auto dTrans = mCmdBuffer->beginRenderPass(mSwapchain, std::move(trans));

    mScene->render(dTrans, cam);

    dTrans->bindShader(mFsq);
    dTrans->bindMaterial(mFsq, mFsqMat);
    dTrans->drawNoBind(3u, 0u);

    trans = mCmdBuffer->endRenderPass(std::move(dTrans));

    mCmdBuffer->end(std::move(trans));

    mGfxDevice->submit(&mCmdBuffer, 1u, mSwapchain);
    mSwapchain->present();
}

void Game::imGuiRender(common::Unique<sw::gfx::CommandTransaction>& trans)
{
    ImGui_ImplSwizzle_UploadFontTexture(trans);

    auto dTrans = mImGuiRenderTarget->beginRenderPass(mCmdBuffer, std::move(trans));
    ImGui::Render();
    ImGui_ImplSwizzle_DrawData(ImGui::GetDrawData(), dTrans);
    trans = mCmdBuffer->endRenderPass(std::move(dTrans));
}
