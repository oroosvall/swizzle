
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

#include <scene/Scene.hpp>

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

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.read(cfgFile.c_str());

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSwizzle_Init(mGfxContext, mWindow);

    mAssetManager = common::CreateRef<AssetManager>(mGfxContext);
    mCompositor = common::CreateRef<Compositor>(mGfxContext, mSwapchain);
    mScene = common::CreateRef<Scene>(mGfxContext, mAssetManager, mCompositor);
    mScene->loadScene("scenes/test.scene");
    mScene->loadSky();
    mScene->loadAnimMesh();

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOn);
    //mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    mCmdBuffer = mGfxContext->createCommandBuffer(2);

    cam.setPosition({ 0.0F, 0.0F, 5.5F });

    sw::gfx::ShaderAttributeList attribFsq = {};
    attribFsq.mEnableBlending = true;

    mFsq = mSwapchain->createShader(attribFsq);
    mFsq->load("shaders/fsq.shader");

    mFsqMat = mGfxContext->createMaterial(mFsq);
    ImGui_ImplSwizzle_SetMaterial(mFsqMat);
}

SwBool Game::userUpdate(F32 dt)
{
    OPTICK_EVENT("userUpdate");

    mFpsCounter.tick(dt);

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
        else if (iter->getType() == sw::gfx::GfxStatsType::InstanceStats)
        {
            sw::gfx::InstanceStatistics* instStats = (sw::gfx::InstanceStatistics*)iter->getStatisticsData();
            title += "Instance\n";
            title += "  Alloc Count " + std::to_string(instStats->mAllocCount) + "\n";
            title += "  Internal Alloc Count " + std::to_string(instStats->mInternalAllocCount) + "\n";
        }

    } while (iter->next());


    title += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    title += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    title += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";

    ImGui_ImplSwizzle_NewFrame(mWindow);
    ImGui::NewFrame();

    ImGui::Begin("Blah");
    {
        OPTICK_EVENT("ImGui::Text");
        ImGui::Text("%s", title.c_str());
    }

    ImGui::End();

    ImGui::EndFrame();

    updateMainWindow(dt);

    return mWindow->isVisible();
}

void Game::userCleanup()
{
    mScene.reset();
    mAssetManager.reset();

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.write(cfgFile.c_str());

    mCmdBuffer.reset();

    mFrameBuffer.reset();

}

void Game::updateMainWindow(F32 dt)
{
    UNUSED_ARG(dt);
    OPTICK_EVENT("Game::updateMainWindow");
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);

    mSwapchain->setClearColor({ 0,0,0,1 });

    mSwapchain->prepare();
    mCmdBuffer->begin();

    mScene->update(dt, mCmdBuffer);

    imGuiRender();

    mCmdBuffer->beginRenderPass(mSwapchain);

    mScene->render(mCmdBuffer, cam);

    mCmdBuffer->bindShader(mFsq);
    mCmdBuffer->bindMaterial(mFsq, mFsqMat);
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
